#include "network_sync_voxel_container_multipleTiles_sender.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/core/byteArray.hpp"
#include "blub/log/global.hpp"
#include "blub/math/octree/search.hpp"
#include "blub/core/string.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/sync/voxel/container/multipleTiles/base.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/serialization/format/binary/output.hpp"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>


using namespace blub::sync::voxel::container::simple;
using namespace blub;


//sender::sender(async::dispatcher &master,
//               const t_octreeSearchCallback &octreeSearch,
//               blub::procedural::voxel::simple::container::base* tiles)
//    : t_base(master, vector3int32(procedural::voxel::tile::container::voxelLength))
//    , m_tiles(tiles)
//    , m_searchFunction(octreeSearch)
//    , m_voxelSize(tiles->getVoxelSize())
//    , m_sizeTile(procedural::voxel::tile::container::voxelLength)
//{
//    BASSERT(tiles != nullptr);

//    m_connTileVoxelEditDone = m_tiles->signalEditDone()->connect(boost::bind(&sender::tileEditDone, this));
//}

//sender::sender(async::dispatcher &master,
//               const t_octreeSearchCallback &octreeSearch,
//               const real &voxelSize)
//    : t_base(master, vector3int32(procedural::voxel::tile::container::voxelLength))
//    , m_tiles(nullptr)
//    , m_searchFunction(octreeSearch)
//    , m_voxelSize(voxelSize)
//    , m_sizeTile(procedural::voxel::tile::container::voxelLength)
//{
//    ;
//}

//sender::~sender()
//{
//    ;
//}


//void sender::tileEditDone()
//{
//    m_tiles->lockForRead();

//    m_master.post(boost::bind(&sender::tileEditDoneMaster, this));
//}

//void sender::addSyncReceiver(sender::t_receiverIdentifierPtr receiver, const vector3 &pos)
//{
//    BASSERT(!receiver.isNull());

//    m_master.post(boost::bind(&sender::addSyncReceiverMaster, this, receiver, pos));
//}

//void sender::updateSyncReceiver(sender::t_receiverIdentifierPtr receiver, const vector3 &pos)
//{
//    BASSERT(!receiver.isNull());

//    m_master.post(boost::bind(&sender::updateSyncReceiverMaster, this, receiver, pos));
//}

//void sender::removeSyncReceiver(sender::t_receiverIdentifierPtr receiver)
//{
//    BASSERT(!receiver.isNull());

//    m_master.post(boost::bind(&sender::removeSyncReceiverMaster, this, receiver));
//}

//sender::t_sigSendTileData *sender::signalSendTileData()
//{
//    return &m_sigSendTileData;
//}

//void sender::tileEditDoneMaster()
//{
//    const sender::t_tileChangedList &affectedTiles(m_tiles->getTilesThatGotEdited());
//    for (auto change : affectedTiles)
//    {
//        t_tileId id(change.first);
//        t_tileHolder workTile(change.second);

//        typedef procedural::voxel::simple::container::utils::tileState t_tileState;

//        t_tileDataMap::iterator it = m_tileData.find(id);
//        const bool found(it != m_tileData.cend());
//        if (found)
//        {
//            if (workTile.state == t_tileState::empty)
//            {
//                tileRemovedMaster(id);
//                continue;
//            }
//            tileChangedMaster(id, workTile);
//            continue;
//        }
//        if (workTile.state == t_tileState::empty)
//        {
//            BASSERT("should not happen");
//            continue;
//        }
//        tileAddedMaster(id, workTile);
//    }

//    m_tiles->unlockRead();

//    endEditingAllMaster();
//}


//void sender::tileAddedMaster(const sender::t_tileId& id, const sender::t_tileHolder& tile)
//{
//    // get and save data
//    BASSERT(m_tileData.find(id) == m_tileData.cend());

//    t_tileDataPtr toSave(new byteArray());
//    compressTile(tile, toSave.data());

//    m_tileData.insert(id, toSave);

//    vector3int32 pos(id*m_syncTree.getMinNodeSize() + m_syncTree.getMinNodeSize()/2);
//    t_base::addSyncMaster(id, vector3(pos));
//}

//void sender::tileChangedMaster(const sender::t_tileId id, const sender::t_tileHolder& tile)
//{
//    // get and save data
//    t_tileDataMap::const_iterator it = m_tileData.find(id);
//    BASSERT(it != m_tileData.cend());

//    t_tileDataPtr toSave(it->second);
//    compressTile(tile, toSave.data());

//    // send updated version
//    {
//        t_syncToReceiversMap::const_iterator itTile = m_syncReceivers.find(id);
//        BASSERT(itTile != m_syncReceivers.cend());
//        const t_receiverList& receivers(itTile->second);
//        for (t_receiverList::const_iterator itRec = receivers.cbegin(); itRec != receivers.cend(); ++itRec)
//        {
//            sendSetTileMaster(*itRec, id, toSave);
//        }
//    }
//}

//void sender::tileRemovedMaster(const sender::t_tileId id)
//{
//    {
//        t_tileDataMap::const_iterator it = m_tileData.find(id);
//        BASSERT(it != m_tileData.cend());
//        m_tileData.erase(it);
//    }
//    {
//        t_base::removeSyncMaster(id);
//    }
//}

//void sender::addSyncReceiverMaster(sender::t_receiverIdentifierPtr receiver, const vector3 &pos)
//{
//    t_base::addReceiverMaster(receiver, pos / m_voxelSize);
//    endEditingAllMaster();
//}

//void sender::updateSyncReceiverMaster(sender::t_receiverIdentifierPtr receiver, const vector3 &pos)
//{
//    t_base::updateReceiverMaster(receiver, pos / m_voxelSize);
//    endEditingAllMaster();
//}

//void sender::removeSyncReceiverMaster(sender::t_receiverIdentifierPtr receiver)
//{
//    t_base::removeReceiverMaster(receiver);
//    endEditingAllMaster();
//}


//void sender::startEditingMaster(sender::t_receiverIdentifierPtr receiver)
//{
//    if (m_receiverInEditMode.find(receiver) != m_receiverInEditMode.cend())
//    {
//        return;
//    }
//    m_receiverInEditMode.insert(receiver);
//    sendStartEditingMaster(receiver);
//}

//void sender::endEditingAllMaster()
//{
//    for (sender::t_receiverIdentifierPtr rec : m_receiverInEditMode)
//    {
//        sendEndEditingMaster(rec);
//    }
//    m_receiverInEditMode.clear();
//}


//void sender::sendStartEditingMaster(sender::t_receiverIdentifierPtr receiver)
//{
//    t_tileDataPtr send(new byteArray(sizeof(sendType)));
//    sendType type(sendType::startEditing);

//    memcpy(send->data(), &type, sizeof(sendType));

//    m_sigSendTileData(receiver, send);
//}

//void sender::sendEndEditingMaster(sender::t_receiverIdentifierPtr receiver)
//{
//    t_tileDataPtr send(new byteArray(sizeof(sendType)));
//    sendType type(sendType::endEditing);

//    memcpy(send->data(), &type, sizeof(sendType));

//    m_sigSendTileData(receiver, send);
//}

//void sender::sendSetTileMaster(sender::t_receiverIdentifierPtr receiver, const sender::t_tileId &id, sender::t_tileDataPtr data)
//{
//    t_tileDataPtr send(new byteArray(data->size() + sizeof(t_tileId) + sizeof(sendType)));
//    sendType type(sendType::setTile);

//    memcpy(send->data(), &type, sizeof(sendType));
//    memcpy(send->data()+sizeof(sendType), &id, sizeof(t_tileId));
//    memcpy(send->data()+sizeof(t_tileId)+sizeof(sendType), data->data(), data->size());

//    m_sigSendTileData(receiver, send);
//}

//void sender::compressTile(const sender::t_tileHolder& tile, byteArray *result)
//{
//    std::stringstream dataContainerCompressed;
//    {
//        if (tile.state == t_tileState::partitial)
//        {
//            boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
//            out.push(boost::iostreams::bzip2_compressor());
//            out.push(dataContainerCompressed);
//            blub::serialization::format::binary::output format_(out);

//            format_ << (*tile.data.data());
//        }
//    }

//    std::string str(dataContainerCompressed.str());
//    int32 sizeCompressed = dataContainerCompressed.str().size();

//    result->clear();
//    result->resize(sizeCompressed + sizeof(t_tileState));
//    memcpy(result->data(), &tile.state, sizeof(t_tileState));
//    memcpy(result->data()+sizeof(t_tileState), str.data(), sizeCompressed);
//}

//bool sender::isInSyncRangeReceiver(const t_base::t_receiver receiver, const vector3 &posOfReceiverLeafCenter, const typename t_base::t_syncTree::t_nodePtr& octreeNode)
//{
//    (void)receiver;
//    return m_searchFunction(posOfReceiverLeafCenter, axisAlignedBox(octreeNode->getBoundingBox()));
//}

//bool sender::isInSyncRangeSync(const t_base::t_sync sync, const vector3 &posOfSyncLeafCenter, const typename t_base::t_receiverTree::t_nodePtr& octreeNode)
//{
//    (void)sync;
//    return m_searchFunction(posOfSyncLeafCenter, axisAlignedBox(octreeNode->getBoundingBox()));
//}

//void sender::addSyncReceiver(const sender::t_receiver receiver, const sender::t_sync sync)
//{
//    startEditingMaster(receiver);
//    t_tileDataMap::const_iterator it(m_tileData.find(sync));
//    BASSERT(it != m_tileData.cend());
//    sendSetTileMaster(receiver, sync, it->second);
//}

//void sender::removeSyncReceiver(const sender::t_receiver receiver, const sender::t_sync sync)
//{
//    startEditingMaster(receiver);

//    t_tileDataPtr toSend(new byteArray(sizeof(t_tileState)));
//    t_tileState state(t_tileState::empty);
//    memcpy(toSend->data(), &state, sizeof(t_tileState));

//    sendSetTileMaster(receiver, sync, toSend);
//}




