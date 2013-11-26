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


class inMemory : public base
{
public:
    typedef base t_base;

    typedef vector3int32map<utils::tile> t_tilesMap;

    inMemory(blub::async::dispatcher &worker, const real& voxelSize);
    virtual ~inMemory();

    utils::tile getTileHolder(const blub::vector3int32& id) const override;

    const t_tilesMap& getTilesMap(void) const;

    void setTileBounds(const axisAlignedBoxInt32& bounds);
    const axisAlignedBoxInt32& getTileBounds() const;

protected:
    void setTileMaster(const blub::vector3int32& id, const utils::tile& toSet) override;

    void setTileToFullMaster(const vector3int32& id) override;
    void setTileToEmtpyMaster(const vector3int32& id) override;

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
                    const utils::tile holder(getTileHolder(id));

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

        lockForEdit();

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

                    utils::tile holder;
                    readWrite & serialization::nameValuePair::create("state", holder.state);

                    if (holder.state == utils::tileState::partitial)
                    {
                        holder.data = createTile(false);
                        readWrite & serialization::nameValuePair::create("tile", *holder.data.data());
                    }

                    setTile(id, holder);
                }
            }
        }

        unlockForEdit();
    }
    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        (void)version;

        readWrite & nameValuePair::create("voxelSize", m_voxelSize);

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
