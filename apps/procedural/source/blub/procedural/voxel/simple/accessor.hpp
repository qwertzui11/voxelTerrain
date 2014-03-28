#ifndef PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP

#include "blub/async/mutexReadWrite.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/log.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/signal.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/math/vector3.hpp"
#include "blub/procedural/voxel/simple/base.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"
#include "blub/procedural/voxel/tile/container.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


/**
 * @brief The accessor class accesses and caches voxels for the surface-calculation.
 * It caches voxel optimized for the marching cubes algorithm.
 * If lod is larger 0 it addionally caches voxel for the transvoxel aalgorithm.
 */
template <class voxelType>
class accessor : public base<sharedPointer<tile::accessor<voxelType> > >
{
public:
    typedef tile::accessor<voxelType> t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;
    typedef base<t_tilePtr> t_base;

    typedef typename t_base::t_tileId t_tileId;
    typedef hashMap<vector3int32, t_tilePtr> t_tiles;

    typedef sharedPointer<tile::container<voxelType> > t_tileContainerPtr;
    typedef hashList<vector3int32> t_tileIdList;
    typedef container::utils::tileState t_tileState;
    typedef container::utils::tile<voxelType> t_tileHolder;
    typedef hashMap<t_tileId, t_tileHolder> t_tileHolderMap;

    typedef container::base<voxelType> t_simpleContainerVoxel;


    /**
     * @brief accessor constructor
     * @param worker May gets run by multiple threads.
     * @param voxels The container to read the voxels from.
     * The Accessor will connect the signal container::base::signalEditDone() to this class, so it keeps up to date.
     * @param lod Indicates the level of detail. Use 0 zero for the most detailed one, use 1 for half detail and so on.
     */
    accessor(async::dispatcher &worker,
             t_simpleContainerVoxel &voxels,
             const int32& lod)
        : t_base(worker)
        , m_voxels(voxels)
        , m_lod(lod)
        , m_voxelSkip(math::pow(2, m_lod))
        , m_numTilesInWork(0)
    {
        m_connTilesGotChanged = m_voxels.signalEditDone()->connect(boost::bind(&accessor::tilesGotChanged, this));

        t_base::setCreateTileCallback(boost::bind(&t_tile::create));
    }

    /**
     * @brief ~accessor destructor
     */
    virtual ~accessor()
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("accessor::~accessor()");
#endif
    }

    /**
     * @brief getVoxelContainer returns the voxel container, set in the constructor.
     * @return Returns never nullptr
     */
    t_simpleContainerVoxel &getVoxelContainer() const
    {
        return m_voxels;
    }

    /**
     * @brief getTile returns an accessor tile.
     * Read-lock class before.
     * @param id TileId
     * @return Returns nullptr if no tile is available.
     */
    t_tilePtr getTile(const t_tileId& id) const
    {
        typename t_tiles::const_iterator it = m_tiles.find(id);
        if (it == m_tiles.cend())
        {
            return nullptr;
        }
        return it->second;
    }

protected:
    /**
     * @brief tilesGotChanged gets called after in the voxel container m_voxels, set in the constructor, the voxels changed.
     * Which leads to a recalculation of the cache.
     */
    void tilesGotChanged()
    {
        m_voxels.lockForRead();
        t_base::m_master.post(boost::bind(&accessor::tilesGotChangedMaster, this));
    }

    /**
     * @brief tilesGotChangedMaster must not get called paralell.
     * @see tilesGotChanged()
     */
    void tilesGotChangedMaster()
    {
        const auto& changedTiles(m_voxels.getTilesThatGotEdited());
        BASSERT(!changedTiles.empty());
        /*if (changedTiles.empty())
        {
            m_voxels.unlockRead();
            return;
        }*/

        t_tileIdList affectedTiles;
        for (auto change : changedTiles)
        {
            const t_tileId id(change.first);
            const t_tileHolder workTile(change.second);

            if (m_lod == 0) // optimisation on lod_0 only
            {
                if (workTile.state == container::utils::tileState::empty)
                {
                    if (m_tiles.find(id) == m_tiles.cend())
                    {
                        continue;
                    }
                }
            }

            calculateAffectedAccessorTilesByContainerTile(id, workTile, affectedTiles);
        }

        if (affectedTiles.empty())
        {
//            blub::BWARNING("affectedTiles.empty()");
            m_voxels.unlockRead();
            return;
        }

        t_base::lockForEditMaster();
        BASSERT(m_numTilesInWork == 0);
        m_numTilesInWork = affectedTiles.size();

        for (const t_tileId id : affectedTiles)
        {
            t_base::m_worker.post(boost::bind(&accessor::calculateAccessorTS, this, id, getTile(id)));
        }
    }

    /**
     * @brief calculateAccessorTS accesses the container and pulls out all voxel needed for surface calculation (marching-cubes/transvoxel).
     * @param id Accessor-TileId
     * @param workTile The accessor-tile to fill with.
     */
    void calculateAccessorTS(const t_tileId& id, t_tilePtr workTile)
    {
        if (workTile.isNull())
        {
            workTile = createTile();
            // workTile->setEmpty();
        }

        const vector3int32 voxelStart(id*t_tile::voxelLength*m_voxelSkip);
        // axisAlignedBoxInt32 lastUsedTileBounds;
        t_tileHolderMap lastUsedTiles;

        bool valuesChanged(false);
        for (int32 indX = -1; indX < t_tile::voxelLength+2; ++indX)
        {
            for (int32 indY = -1; indY < t_tile::voxelLength+2; ++indY)
            {
                for (int32 indZ = -1; indZ < t_tile::voxelLength+2; ++indZ)
                {
                    const vector3int32 pos(indX, indY, indZ);
                    const vector3int32 voxelPosAbs(voxelStart + pos*m_voxelSkip);

                    valuesChanged |= workTile->setVoxel(pos, getVoxelData(voxelPosAbs, /*lastUsedTileBounds, */lastUsedTiles));
                }
            }
        }

        if (workTile->getCalculateLod())
        {
            BASSERT(m_voxelSkip > 0);

            const int32 voxelLengthLod = t_tile::voxelLengthLod;
            const vector3int32 toIterate[][2] = {
                                                {{0, 0, 0}, {1, voxelLengthLod, voxelLengthLod}},
                                                {{voxelLengthLod-2, 0, 0}, {voxelLengthLod-1, voxelLengthLod, voxelLengthLod}},
                                                {{0, 0, 0}, {voxelLengthLod, 1, voxelLengthLod}},
                                                {{0, voxelLengthLod-2, 0}, {voxelLengthLod, voxelLengthLod-1, voxelLengthLod}},
                                                {{0, 0, 0}, {voxelLengthLod, voxelLengthLod, 1}},
                                                {{0, 0, voxelLengthLod-2}, {voxelLengthLod, voxelLengthLod, voxelLengthLod-1}},
                                                };
            for (int32 lod = 0; lod < 6; ++lod)
            {
                const vector3int32& start(toIterate[lod][0]);
                const vector3int32& end(toIterate[lod][1]);
                for (int32 indX = start.x; indX < end.x; ++indX)
                {
                    for (int32 indY = start.y; indY < end.y; ++indY)
                    {
                        for (int32 indZ = start.z; indZ < end.z; ++indZ)
                        {
                            const vector3int32 pos(indX, indY, indZ);
                            const vector3int32 voxelPosAbs(voxelStart + pos*(m_voxelSkip/2));

                            const voxelType result(getVoxelData(voxelPosAbs, /*lastUsedTileBounds, */lastUsedTiles));
    #ifdef BLUB_DEBUG
                            if (indX % 2 == 0 &&
                                indY % 2 == 0 &&
                                indZ % 2 == 0)
                            {
                                BASSERT(workTile->getVoxel(pos / 2) == result);
                            }
    #endif

                            valuesChanged |= workTile->setVoxelLod(pos-start, result, lod);
                        }
                    }
                }
            }
        }

        if (workTile->isEmpty() || workTile->isFull())
        {
            t_base::m_master.post(boost::bind(&accessor::afterCalculateAccessorMaster, this, id, nullptr, true));
            return;
        }

        t_base::m_master.post(boost::bind(&accessor::afterCalculateAccessorMaster, this, id, workTile, valuesChanged));
    }

    /**
     * @brief afterCalculateAccessorMaster gets called after a worker-thread finished calculation of a part.
     * @param id TileId
     * @param workTile The resulting accessor-tile.
     * @param didValuesChanged Tells if anything changed.
     */
    void afterCalculateAccessorMaster(const t_tileId& id, t_tilePtr workTile, const bool &didValuesChanged)
    {
        BASSERT(t_base::getTilesThatGotEdited().find(id) == t_base::getTilesThatGotEdited().cend());

        typename t_tiles::const_iterator it = m_tiles.find(id);

        // no indices
        if (workTile.isNull())
        {
            if (it != m_tiles.cend())
            {
                m_tiles.erase_return_void(it);
                t_base::addToChangeList(id, nullptr);
            }
        }
        else
        {
            BASSERT(!workTile.isNull());
            if (it == m_tiles.cend())
            {
                m_tiles.insert(id, workTile);
            }
            if (didValuesChanged)
            {
                t_base::addToChangeList(id, workTile);
            }
        }


        --m_numTilesInWork;
        if (m_numTilesInWork == 0)
        {
            m_voxels.unlockRead();
            t_base::unlockForEditMaster();
        }
    }


    /**
     * @brief When a tile in container gets changed it affects (because of normal-correction and lod) 3^3 accessor-tiles.
     * @param conterainerId Container-Tile-Id
     * @param holder Container-Data
     * @param resultingSurfaceTiles Resulting, up to 27, to recalculate tiles.
     * Depending on how the change-axisAligendBox in the container-tile looks like.
     */
    void calculateAffectedAccessorTilesByContainerTile(const t_tileId& conterainerId, const t_tileHolder &holder, t_tileIdList& resultingSurfaceTiles)
    {
        const vector3 forDiv(conterainerId);
        const vector3int32 result((forDiv / (real)m_voxelSkip).getFloor());
        resultingSurfaceTiles.insert(result);

        vector3int32 minimum(0);
        if (holder.state == t_tileState::partitial)
        {
            BASSERT(holder.data->getEditedVoxelBoundingBox().isValid());
            minimum = holder.data->getEditedVoxelBoundingBox().getMinimum();
        }
        if (minimum > vector3int32(0))
        {
            return;
        }

        const vector3int32 resultMod(conterainerId%m_voxelSkip);
        if (resultMod.x == 0 && minimum.x == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(1, 0, 0));
        }
        if (resultMod.y == 0 && minimum.y == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(0, 1, 0));
        }
        if (resultMod.z == 0 && minimum.z == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(0, 0, 1));
        }
        if (resultMod.x == 0 && resultMod.y == 0 && minimum.x == 0 && minimum.y == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(1, 1, 0));
        }
        if (resultMod.x == 0 && resultMod.z == 0 && minimum.x == 0 && minimum.z == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(1, 0, 1));
        }
        if (resultMod.y == 0 && resultMod.z == 0 && minimum.y == 0 && minimum.z == 0)
        {
            resultingSurfaceTiles.insert(result - vector3int32(0, 1, 1));
        }
        if (resultMod.x == 0 && resultMod.y == 0 && resultMod.z == 0 && minimum == vector3int32(0))
        {
            resultingSurfaceTiles.insert(result - vector3int32(1, 1, 1));
        }
    }

    /**
     * @brief createTile creates an empty tile.
     * @return
     */
    t_tilePtr createTile() const override
    {
        t_tilePtr result(t_base::createTile());
        result->setCalculateLod(m_lod > 0);
        return result;
    }

    /**
     * @brief getVoxelData returns a voxel from a cahned container-tile or looks up the tile and returns it.
     * @param voxelPosAbs Absolut voxel position.
     * @param lastUsedTiles Last used container-tile.
     * @return Always a valid voxel. If not found a default-constructed voxel.
     */
    voxelType getVoxelData(const vector3int32& voxelPosAbs, t_tileHolderMap& lastUsedTiles)
    {
        const vector3int32 &tilePos(m_voxels.calculateVoxelPosToTileId(voxelPosAbs));
        const vector3int32 &tilePosAbs(tilePos*vector3int32(t_tile::voxelLength));

        typename t_tileHolderMap::const_iterator it = lastUsedTiles.find(tilePos);

        voxelType result;
        if (it != lastUsedTiles.cend())
        {
            const t_tileHolder &lastUsedTile(it->second);
            switch(lastUsedTile.state)
            {
            case t_tileState::partitial:
                result = lastUsedTile.data->getVoxel(voxelPosAbs-tilePosAbs);
                break;
            case t_tileState::empty:
                result.setMin();
                break;
            case t_tileState::full:
                result.setMax();
                break;
            default:
                BASSERT(false);
            }
            return result;
        }

        // get new tile
        const t_tileHolder &lastUsedTile(m_voxels.getTileHolder(tilePos));
        lastUsedTiles.insert(tilePos, lastUsedTile);

        // repeat step ahead
        switch(lastUsedTile.state)
        {
        case t_tileState::partitial:
            result = lastUsedTile.data->getVoxel(voxelPosAbs-tilePosAbs);
            break;
        case t_tileState::empty:
            result.setMin();
            break;
        case t_tileState::full:
            result.setMax();
            break;
        default:
            BASSERT(false);
        }
        return result;
    }

protected:

    t_simpleContainerVoxel& m_voxels;
    const int32 m_lod;
    const int32 m_voxelSkip;
    int32 m_numTilesInWork;

    t_tiles m_tiles;

    boost::signals2::scoped_connection m_connTilesGotChanged;
};


}
}
}
}


#endif // PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP
