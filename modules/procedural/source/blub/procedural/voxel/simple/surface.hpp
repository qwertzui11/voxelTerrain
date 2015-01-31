#ifndef PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP
#define PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP


#include "blub/core/globals.hpp"
#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/signal.hpp"
#include "blub/math/vector3int.hpp"
#include "blub/procedural/log/global.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"

#include <boost/signals2/connection.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


/**
 * @brief The surface class convertes accessor-tiles to surface-tiles. In between polygons get calculated by the surface-tile.
 */
template <class configType>
class surface : public base<typename configType::t_surface::t_tile>
{
public:
    typedef configType t_config;
    typedef typename t_config::t_surface::t_tile t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;
    typedef base<t_tile> t_base;

    typedef typename t_base::t_tileId t_tileId;

    typedef hashMap<t_tileId, t_tilePtr> t_tilesMap;
    typedef hashList<vector3int32> t_tileIdList;

    typedef typename t_config::t_accessor::t_tile t_tileAccessor;
    typedef sharedPointer<t_tileAccessor> t_tileAccessorPtr;
    typedef base<t_tileAccessor> t_voxelAccessor;


    /**
     * @brief surface constructor.
     * @param worker May getting called by several threads.
     * @param voxels The accessor to which this class listens for updates to.
     * @param lod Indicates the lod. 0 for highest detail, 1 for half detail and so on.
     */
    surface(blub::async::dispatcher &worker,
            t_voxelAccessor& voxels,
            const int32& lod)
        : t_base(worker)
        , m_voxels(voxels)
        , m_lod(lod)
        , m_numTilesInWork(0)
    {
        voxels.signalEditDone()->connect(boost::bind(&surface::editDone, this));

        t_base::setCreateTileCallback(boost::bind(&t_tile::create));
    }
    /**
     * @brief ~surface destructor.
     */
    ~surface()
    {
        ;
    }

    /**
     * @brief getTileCount returns the tile count calculated by this class. Read-lock class before calling.
     * @return Always a positive value.
     */
    int32 getTileCount() const
    {
        return m_tiles.size();
    }
    /**
     * @brief getVoxelSize returns the voxel-size.
     * @return pow(2, lod)
     */
    real getVoxelSize() const
    {
        return math::pow(2., m_lod);
    }

    /**
     * @brief getTile returns a surface-tile. Lock-read class before.
     * @param id TileId
     * @return If not found returns nullptr.
     */
    t_tilePtr getTile(const blub::vector3int32& id) const
    {
        typename t_tilesMap::const_iterator it(m_tiles.find(id));
        if (it == m_tiles.cend())
        {
            return nullptr;
        }
        return it->second;
    }

    /**
     * @brief getOrCreateTile returns same like getTile() except that when not found instances a new tile.
     * @param id TileId.
     * @return Never nullptr.
     * @see getTile()
     */
    t_tilePtr getOrCreateTile(const blub::vector3int32& id) const
    {
        t_tilePtr workTile(getTile(id));
        if (workTile.isNull())
        {
            return t_base::createTile();
        }
        return workTile;
    }

protected:
    /**
     * @brief editDone gets called when data in accessor changed.
     */
    void editDone()
    {
        m_voxels.lockForRead();

        t_base::m_master.post(boost::bind(&surface::editDoneMaster, this));
    }

    /**
     * @brief editDoneMaster same like editDone() but on master-thread.
     * Write locks class, dispatches surface generation to worker threads.
     * @see editDone()
     */
    void editDoneMaster()
    {
        const typename t_voxelAccessor::t_tilesGotChangedMap& change(m_voxels.getTilesThatGotEdited());
#ifdef BLUB_LOG_VOXEL
        BLUB_PROCEDURAL_LOG_OUT() << "surface editDoneMaster change.size():" << change.size();
#endif
        if (change.empty())
        {
            BLUB_PROCEDURAL_LOG_WARNING() << "change.empty()";
            return;
        }

        t_base::lockForEditMaster();

        BASSERT(m_numTilesInWork == 0);
        m_numTilesInWork = change.size();

        for (auto work : change)
        {
            if (work.second.isNull())
            {
                afterCalculateSurfaceMaster(work.first, nullptr);
                continue;
            }

            BASSERT(!work.second->isEmpty());
            BASSERT(!work.second->isFull());

            t_tilePtr workTile(getTile(work.first));

            t_base::m_worker.post(boost::bind(&surface::calculateSurfaceTS, this, work.first, work.second, workTile));
        }
    }

    /**
     * @brief calculateSurfaceTS gets called by editDoneMaster(), by any worker-thread.
     * Calls afterCalculateSurfaceMaster() after work is done.
     * @param id TileId
     * @param work The accessorTile to turn into a surface-tile.
     * @param workTile the resulting surface tile.
     * @see editDoneMaster()
     */
    void calculateSurfaceTS(const t_tileId id, t_tileAccessorPtr work, t_tilePtr workTile)
    {
        if (workTile.isNull())
        {
            workTile = t_base::createTile();
        }
        workTile->calculateSurface(work,
                                   getVoxelSize(),
                                   true,
                                   m_lod);

        if (workTile->getIndices().empty())
        {
#ifdef BLUB_LOG_VOXEL
            BLUB_PROCEDURAL_LOG_WARNING() << "workTile->getIndices().empty() id:" << id << " m_lod:" << m_lod << " work->getNumVoxelLargerZero():" << work->getNumVoxelLargerZero();
#endif
            t_base::m_master.post(boost::bind(&surface::afterCalculateSurfaceMaster, this, id, nullptr));
            return;
        }

        t_base::m_master.post(boost::bind(&surface::afterCalculateSurfaceMaster, this, id, workTile));
    }

    /**
     * @brief afterCalculateSurfaceMaster gets called by calculateSurfaceTS() on master-thread.
     * @param id TileId
     * @param workTile The reulting surface-tile. If no polygons got created it is nullptr.
     * @see calculateSurfaceTS()
     */
    void afterCalculateSurfaceMaster(const t_tileId& id, t_tilePtr workTile)
    {
#ifdef BLUB_LOG_VOXEL
        BLUB_LOG_OUT() << "afterCalculateSurfaceMaster id:" << id;
#endif

        typename t_tilesMap::const_iterator it(m_tiles.find(id));

        int32 numIndices(0);
        if (!workTile.isNull())
        {
            numIndices = workTile->getIndices().size();
        }

        // no indices
        if (numIndices == 0)
        {
            if (it != m_tiles.cend())
            {
                t_base::addToChangeList(id, nullptr);
                m_tiles.erase(it);
            }
        }
        else
        {
            BASSERT(!workTile.isNull());
            if (it == m_tiles.cend())
            {
                m_tiles.insert(id, workTile);
            }
            t_base::addToChangeList(id, workTile);
        }

        --m_numTilesInWork;
        BASSERT(m_numTilesInWork >= 0);
        if (m_numTilesInWork == 0)
        {
            m_voxels.unlockRead();
            t_base::unlockForEditMaster();
        }
    }


private:
    t_tilesMap m_tiles;

    t_voxelAccessor &m_voxels;
    int32 m_lod;
    int32 m_numTilesInWork;

    boost::signals2::scoped_connection m_connTilesGotChanged;

};


}
}
}
}


#endif // PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP
