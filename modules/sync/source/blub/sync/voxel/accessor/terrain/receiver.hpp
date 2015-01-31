#ifndef NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_RECEIVER_HPP
#define NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_RECEIVER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/byteArray.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"
#include "blub/sync/voxel/accessor/multipleTiles/receiver.hpp"


namespace blub
{
namespace sync
{
namespace voxel
{
namespace accessor
{
namespace terrain
{


template <class voxelType>
class receiver : public procedural::voxel::terrain::base< sharedPointer<procedural::voxel::tile::accessor<voxelType> > >
{
public:
    typedef multipleTiles::receiver<voxelType> t_multipleTiles;
    typedef t_multipleTiles* t_multipleTilesPtr;

    typedef procedural::voxel::terrain::base< sharedPointer<procedural::voxel::tile::accessor<voxelType> > > t_base;

    typedef sharedPointer<byteArray> t_tileDataPtr;
    typedef vector<t_multipleTilesPtr> t_multipleTilesList;

    receiver(async::dispatcher *todoListenerMaster, const uint16 &numLod)
    {
        for (int32 ind = 0; ind < numLod; ++ind)
        {
            t_multipleTilesPtr lod(new t_multipleTiles(todoListenerMaster, ind));
            t_base::m_lods.push_back(lod);
        }
    }
    virtual ~receiver()
    {
        for (auto toDelete : t_base::m_lods)
        {
            delete toDelete;
        }
        t_base::m_lods.clear();
    }

    // "receive sync"
    void receivedTileData(const byteArray& data)
    {
        BASSERT((uint32)data.size() > sizeof(uint32));

        uint32 lodIndex;
        byteArray restOftheData;

        std::istringstream input(std::string(data.data(), data.size()));
        blub::serialization::format::binary::input format(input);

        format >> lodIndex;
        format >> restOftheData;

        static_cast<t_multipleTilesPtr>(t_base::m_lods[lodIndex])->receivedTileData(restOftheData);
    }
    void receivedTilePtrData(t_tileDataPtr data)
    {
        receivedTileData(*data);
    }

private:


};


}
}
}
}
}



#endif // NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_RECEIVER_HPP
