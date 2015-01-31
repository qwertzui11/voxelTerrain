#ifndef VOXEL_SIMPLE_CONTAINER_INMEMORY_HPP
#define VOXEL_SIMPLE_CONTAINER_INMEMORY_HPP


#include "blub/math/vector3int32map.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"
#include "blub/serialization/saveLoad.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{
namespace container
{


/**
 * @brief The inMemory class stores all voxels in instances of tile::container and safes them in a very fast in-memory-container.
 * Accessing a voxel-container costs O(1).
 * The class does not save tiles that are full (container-tile returns true for isEmpty() ) or that are empty (container-tile returns true for isFull() ).
 * Instead the class saves the state empty/full.
 * Tiles that are full or empty dont produce a surface.
 */
template <class configType>
class inMemory : public base<configType>
{
public:
    typedef base<configType> t_base;

    typedef vector3int32map<typename t_base::t_utilsTile> t_tilesMap;

    /**
     * @brief inMemory constructor
     * @param worker May gets called by several threads.
     */
    inMemory(blub::async::dispatcher &worker)
        : t_base(worker)
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("inMemory::inMemory()");
#endif
    }

    /**
     * @brief ~inMemory descructor
     */
    ~inMemory()
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("inMemory::~inMemory()");
    #endif
    }

    /**
     * @brief getTileHolder returns a utils::tileHolder setted by setTile() or by editVoxel(). Read-lock class before call.
     * Read-lock the class before.
     * @param id Identifier. Contains voxel from id*blub::procedural::voxel::tile::container::voxelLength to (id+1)*blub::procedural::voxel::tile::container::voxelLength-1
     * @return Always returns a valid value. If nothing found state in utils::tile gets set to empty.
     */
    typename t_base::t_utilsTile getTileHolder(const blub::vector3int32& id) const override
    {
        const axisAlignedBoxInt32 bounds(m_tiles.getBounds().getMinimum(), m_tiles.getBounds().getMaximum() - vector3int32(1));
        if (bounds.isInside(id))
        {
            return m_tiles.getValue(id);
        }
        typename t_base::t_utilsTile result;
        return result;
    }

    /**
     * @brief getTilesMap returns all tiles.
     * @return
     */
    const t_tilesMap& getTilesMap() const
    {
        return m_tiles;
    }

    /**
     * @brief setTileBounds sets the size for the map. Call for optimization, if you know the tile-dimensions before editing.
     * @param bounds
     */
    void setTileBounds(const axisAlignedBoxInt32& bounds)
    {
        m_tiles.resize(bounds);
    }

    /**
     * @brief getTileBounds returns the tile bounds.
     * @return
     */
    const axisAlignedBoxInt32& getTileBounds() const
    {
        return m_tiles.getBounds();
    }

protected:
    /**
     * @brief setTileToContainerMaster replaces a tile. Call method by one thread at a time. Write-lock class before.
     * @param id TileId
     * @param oldOne Not used - may be interesting in derived classes.
     * @param toSet
     */
    virtual void setTileToContainerMaster(const typename t_base::t_tileId id, const typename t_base::t_utilsTile &oldOne, const typename t_base::t_utilsTile &toSet)
    {
        (void)oldOne;

        m_tiles.extend(axisAlignedBoxInt32(id, id+vector3int32(1)));
        m_tiles.setValue(id, toSet);
    }

    /**
     * @brief setTileMaster sets a tile to an id. Call by one thread at a time.
     * @param id TileId
     * @param toSet
     */
    void setTileMaster(const blub::vector3int32& id, const typename t_base::t_utilsTile& toSet) override
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("inMemory::setTileMaster id:" + blub::string::number(id));
#endif

        typename t_base::t_utilsTile holder(getTileHolder(id));
        bool alreadyEmpty(holder.state == utils::tileState::empty);

        if (toSet.state == utils::tileState::empty && alreadyEmpty)
        {
            return;
        }
        if (!alreadyEmpty)
        {
            if (toSet.state == utils::tileState::full &&
                holder.state == utils::tileState::full) // nothing will change
            {
                return;
            }
        }
        setTileToContainerMaster(id, holder, toSet);

        if (toSet.state == utils::tileState::partitial)
        {
            BASSERT(!toSet.data->isEmpty());
            BASSERT(!toSet.data->isFull());

            if (!toSet.data->getEditedVoxelBoundingBox().isValid()) // no voxel changed
            {
                toSet.data->endEdit();
                return;
            }
        }
        t_base::addToChangeList(id, toSet);
    }

    void setTileToFullMaster(const vector3int32& id) override
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("inMemory::setTileToFullMaster id:" + blub::string::number(id));
#endif
        const typename t_base::t_utilsTile holder(utils::tileState::full);
        setTileMaster(id, holder);
    }
    void setTileToEmtpyMaster(const vector3int32& id) override
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("inMemory::setTileToEmtpyMaster id:" + blub::string::number(id));
#endif
        const typename t_base::t_utilsTile holder(utils::tileState::empty);
        setTileMaster(id, holder);
    }

protected:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void save(formatType & readWrite, const uint32& version) const
    {
        (void)version;

        const axisAlignedBoxInt32& bounds(getTileBounds());
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(bounds);

        for (int32 indX = bounds.getMinimum().x; indX < bounds.getMaximum().x; ++indX)
        {
            for (int32 indY = bounds.getMinimum().y; indY < bounds.getMaximum().y; ++indY)
            {
                for (int32 indZ = bounds.getMinimum().z; indZ < bounds.getMaximum().z; ++indZ)
                {
                    const vector3int32 id(indX, indY, indZ);
                    const typename t_base::t_utilsTile holder(getTileHolder(id));

                    readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(id);
                    readWrite & serialization::nameValuePair::create("state", holder.state);

                    if (holder.state != utils::tileState::partitial)
                    {
                        continue;
                    }

                    readWrite & serialization::nameValuePair::create("tile", *holder.data.data());
                }
            }
        }
    }
    template <class formatType>
    void load(formatType & readWrite, const uint32& version)
    {
        (void)version;

        t_base::lockForEdit();

        axisAlignedBoxInt32 bounds;
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(bounds);

        setTileBounds(bounds);

        for (int32 indX = bounds.getMinimum().x; indX < bounds.getMaximum().x; ++indX)
        {
            for (int32 indY = bounds.getMinimum().y; indY < bounds.getMaximum().y; ++indY)
            {
                for (int32 indZ = bounds.getMinimum().z; indZ < bounds.getMaximum().z; ++indZ)
                {
                    vector3int32 id;
                    readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(id);
                    BASSERT(id == vector3int32(indX, indY, indZ));

                    typename t_base::t_utilsTile holder;
                    readWrite & serialization::nameValuePair::create("state", holder.state);

                    if (holder.state == utils::tileState::partitial)
                    {
                        holder.data = t_base::createTile();
                        readWrite & serialization::nameValuePair::create("tile", *holder.data.data());
                    }

                    setTile(id, holder);
                }
            }
        }

        t_base::unlockForEdit();
    }
    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        saveLoad(readWrite, *this, version);
    }

private:
    t_tilesMap m_tiles;

};


}
}
}
}
}


#endif // VOXEL_SIMPLE_CONTAINER_INMEMORY_HPP
