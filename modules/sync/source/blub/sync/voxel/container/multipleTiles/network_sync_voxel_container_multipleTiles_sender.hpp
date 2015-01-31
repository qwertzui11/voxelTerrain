#ifndef NETWORK_SYNC_VOXEL_CONTAINER_MULTIPLETILES_SENDER_HPP
#define NETWORK_SYNC_VOXEL_CONTAINER_MULTIPLETILES_SENDER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/hashMapMulti.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/core/signal.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"
#include "blub/sync/sender.hpp"

#include <boost/signals2/connection.hpp>
#include <boost/function/function2.hpp>


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


//class sender : protected ::blub::sync::sender<vector3int32, sharedPointer<identifier> >
//{
//public:
//    typedef vector3int32 t_tileId;
//    typedef sharedPointer<byteArray> t_tileDataPtr;
//    typedef sharedPointer<procedural::voxel::tile::container> t_tilePtr;

//    typedef ::blub::sync::sender<t_tileId, sharedPointer<identifier> > t_base;

//    typedef sharedPointer<identifier> t_receiverIdentifierPtr;
//    typedef hashMap<t_tileId, t_tileDataPtr> t_tileDataMap;
//    typedef procedural::voxel::simple::container::utils::tile t_tileHolder;
//    typedef procedural::voxel::simple::container::utils::tileState t_tileState;
//    typedef hashMap<t_tileId, t_tileHolder> t_tileChangedList;

//    typedef boost::function<bool (vector3, axisAlignedBox)> t_octreeSearchCallback;

//    sender(async::dispatcher &master,
//           const t_octreeSearchCallback& octreeSearch,
//           blub::procedural::voxel::simple::container::base* tiles);
//    sender(blub::async::dispatcher &master,
//           const t_octreeSearchCallback& octreeSearch,
//           const real& voxelSize);
//    virtual ~sender();

//    // to "send sync"
//    void tileEditDone();

//    // add/update/remove sync-reveiver
//    void addSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos);
//    void updateSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos);
//    void removeSyncReceiver(t_receiverIdentifierPtr receiver);

//    // to "send sync" signals
//    typedef blub::signal<void (t_receiverIdentifierPtr, t_tileDataPtr)> t_sigSendTileData;
//    t_sigSendTileData* signalSendTileData(void);

//protected:
//    void tileEditDoneMaster();
//    void tileAddedMaster(const t_tileId &id, const t_tileHolder &tile);
//    void tileChangedMaster(const t_tileId id, const t_tileHolder &tile);
//    void tileRemovedMaster(const t_tileId id);

//    void addSyncReceiverMaster(t_receiverIdentifierPtr receiver, const vector3& pos);
//    void updateSyncReceiverMaster(t_receiverIdentifierPtr receiver, const vector3& pos);
//    void removeSyncReceiverMaster(t_receiverIdentifierPtr receiver);

//    void updateLinkReceiverTilesMaster(t_receiverIdentifierPtr receiver);
//    void addLinkTileReceiverMaster(t_receiverIdentifierPtr receiver, const t_tileId &tile);
//    void removeLinkTileReceiverMaster(t_receiverIdentifierPtr receiver, const t_tileId &tile, const bool& callSignals = true);

//    void startEditingMaster(t_receiverIdentifierPtr receiver);
//    void endEditingAllMaster(void);

//    void sendStartEditingMaster(t_receiverIdentifierPtr receiver);
//    void sendEndEditingMaster(t_receiverIdentifierPtr receiver);
//    void sendSetTileMaster(t_receiverIdentifierPtr receiver, const t_tileId &id, t_tileDataPtr data);

//    static void compressTile(const t_tileHolder &tile, byteArray* result);

//    virtual bool isInSyncRangeReceiver(const t_base::t_receiver receiver, const vector3 &posOfReceiverLeafCenter, const typename t_base::t_syncTree::t_nodePtr& octreeNode);
//    virtual bool isInSyncRangeSync(const t_base::t_sync sync, const vector3 &posOfSyncLeafCenter, const typename t_base::t_receiverTree::t_nodePtr& octreeNode);
//    virtual void addSyncReceiver(const t_receiver receiver, const t_sync sync);
//    virtual void removeSyncReceiver(const t_receiver receiver, const t_sync sync);


//    procedural::voxel::simple::container::base *m_tiles;
//    t_octreeSearchCallback m_searchFunction;
//    real m_voxelSize;
//    const vector3 m_sizeTile;

//    t_tileDataMap m_tileData;

//    t_sigSendTileData m_sigSendTileData;

//    t_receiverList m_receiverInEditMode;

//    boost::signals2::scoped_connection m_connTileVoxelEditDone;

//};


}
}
}
}
}


#endif // NETWORK_SYNC_VOXEL_CONTAINER_SENDER_HPP
