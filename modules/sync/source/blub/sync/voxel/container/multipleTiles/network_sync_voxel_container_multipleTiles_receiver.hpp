#ifndef NETWORK_SYNC_VOXEL_CONTAINER_MULTIPLETILES_RECEIVER_HPP
#define NETWORK_SYNC_VOXEL_CONTAINER_MULTIPLETILES_RECEIVER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/signal.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"



namespace blub
{
namespace sync
{
namespace voxel
{
namespace container
{
namespace simple
{


//class receiver
//{
//public:
//    typedef sharedPointer<byteArray> t_tileDataPtr;
//    typedef sharedPointer<procedural::voxel::tile::container> t_tilePtr;
//    typedef procedural::voxel::simple::container::utils::tile t_tileHolder;
//    typedef vector3int32 t_tileId;
//    typedef hashList<t_tileId> t_tileList;

//    receiver(blub::async::dispatcher * todoListenerMaster, procedural::voxel::simple::container::base *tiles = nullptr);

//    // "receive sync"
//    void receivedTileData(const byteArray& data);
//    void receivedTilePtrData(t_tileDataPtr data);

//    // to "receive sync" signals
//    typedef blub::signal<void ()> t_sigStartEditing;
//    t_sigStartEditing* signalStartEditing(void);
//    typedef blub::signal<void ()> t_sigEndEditing;
//    t_sigEndEditing* signalEndEditing(void);
//    typedef blub::signal<void (t_tileId, t_tileHolder)> t_sigSetTile;
//    t_sigSetTile* signalSetTile(void);

//protected:
//    void receivedTileDataMaster(const byteArray& data);
//    void receivedTileDataPtrMaster(t_tileDataPtr data);

//    procedural::voxel::simple::container::base *m_tiles;
//    async::dispatcher *m_todoListenerMaster;

//    t_sigStartEditing m_sigStartEditing;
//    t_sigEndEditing m_sigEndEditing;
//    t_sigSetTile m_sigSetTile;

//};




}
}
}
}
}



#endif // NETWORK_SYNC_VOXEL_CONTAINER_MULTIPLETILES_RECEIVER_HPP
