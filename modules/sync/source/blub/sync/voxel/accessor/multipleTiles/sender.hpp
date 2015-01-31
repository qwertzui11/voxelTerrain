#ifndef NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_SENDER_HPP
#define NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_SENDER_HPP

#include "blub/async/dispatcher.hpp"
#include "blub/core/byteArray.hpp"
#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/signal.hpp"
#include "blub/log/global.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/octree/search.hpp"
#include "blub/procedural/predecl.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"
#include "blub/sync/log/global.hpp"
#include "blub/sync/sender.hpp"
#include "blub/sync/voxel/accessor/multipleTiles/base.hpp"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/function/function2.hpp>
#include <boost/signals2/connection.hpp>


namespace blub
{
namespace sync
{
namespace voxel
{
namespace accessor
{
namespace multipleTiles
{


template <class voxelType, class identifierType>
class sender : protected ::blub::sync::sender<vector3int32, sharedPointer<identifierType> >
{
public:
    typedef identifierType t_identifier;
    typedef vector3int32 t_tileId;
    typedef sharedPointer<byteArray> t_tileDataPtr;
    typedef procedural::voxel::tile::container<voxelType> t_tileContainer;
    typedef sharedPointer<t_tileContainer> t_tilePtr;

    typedef blub::sync::sender<t_tileId, sharedPointer<t_identifier> > t_base;

    typedef sharedPointer<t_identifier> t_receiverIdentifierPtr;
    typedef hashMap<t_tileId, t_tileDataPtr> t_tileDataMap;

    typedef procedural::voxel::tile::accessor<voxelType> t_tileAccessor;
    typedef sharedPointer<t_tileAccessor> t_tileAccessorPtr;
    typedef hashMap<t_tileId, t_tileAccessorPtr> t_tileAccessorChangeList;

    typedef hashList<t_receiverIdentifierPtr> t_lockedReceiverList;

    typedef boost::function<bool (vector3, axisAlignedBox)> t_octreeSearchCallback;

    typedef procedural::voxel::simple::base<sharedPointer<procedural::voxel::tile::accessor<voxelType> > > t_multipleTilesAccessor;


    sender(async::dispatcher &worker, const real &voxelSize, const t_octreeSearchCallback& octreeSearch, t_multipleTilesAccessor* tiles)
        : t_base(worker, vector3int32(t_tileContainer::voxelLength))
        , m_worker(worker)
        , m_voxels(tiles)
        , m_searchFunction(octreeSearch)
        , m_voxelSize(voxelSize)
        , m_numtilesInWork(0)
    {
        BASSERT(tiles != nullptr);

        m_connTileVoxelEditDone = m_voxels->signalEditDone()->connect(boost::bind(&sender::tileEditDone, this));
    }
    virtual ~sender()
    {
        ;
    }

    // to "send sync"
    void tileEditDone()
    {
        BASSERT(m_voxels != nullptr);

        m_voxels->lockForRead();

        t_base::m_master.post(boost::bind(&sender::tileEditDoneMaster, this));
    }

    // add/update/remove sync-reveiver
    void addSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        BASSERT(!receiver.isNull());

        t_base::m_master.post(boost::bind(&sender::addSyncReceiverMaster, this, receiver, pos));
    }
    void updateSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        BASSERT(!receiver.isNull());

        t_base::m_master.post(boost::bind(&sender::updateSyncReceiverMaster, this, receiver, pos));
    }
    void removeSyncReceiver(t_receiverIdentifierPtr receiver)
    {
        BASSERT(!receiver.isNull());

        t_base::m_master.post(boost::bind(&sender::removeSyncReceiverMaster, this, receiver));
    }

    // to "send sync" signals
    typedef blub::signal<void (t_receiverIdentifierPtr, t_tileDataPtr)> t_sigSendTileData;
    t_sigSendTileData* signalSendTileData()
    {
        return &m_sigSendTileData;
    }

protected:
    void tileEditDoneMaster()
    {
        const t_tileAccessorChangeList& changeList(m_voxels->getTilesThatGotEdited());

        BASSERT(m_numtilesInWork == 0);
        m_numtilesInWork = changeList.size();

        for (auto change : changeList)
        {
            t_tileId id(change.first);
            t_tileAccessorPtr workTile(change.second);

            if (workTile.isNull()) // when empty/full
            {
                compressTileAfterMaster(id, workTile, nullptr);
                continue;
            }
            t_tileDataMap::const_iterator it = m_tileData.find(id);
            if (it == m_tileData.cend())
            {
                BASSERT(!workTile->isEmpty());
                BASSERT(!workTile->isFull());
                m_worker.post(boost::bind(&sender::compressTileWorker, this, id, workTile, nullptr));
                continue;
            }
            m_worker.post(boost::bind(&sender::compressTileWorker, this, id, workTile, it->second));
        }
    }

    void addSyncReceiverMaster(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        t_base::addReceiverMaster(receiver, pos / m_voxelSize);

        unlockAllReceiver();
    }
    void updateSyncReceiverMaster(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        if (t_base::m_receiverPosMap.find(receiver) == t_base::m_receiverPosMap.cend())
        {
            BLUB_SYNC_LOG_WARNING() << "m_receiverPosMap.find(receiver) == m_receiverPosMap.cend()";
            return;
        }
        //blub::BOUT("sender::updateSyncReceiverMaster m_voxelSize:" + blub::string::number(m_voxelSize)
        //           + " pos:" + blub::string::number(pos));
        t_base::updateReceiverMaster(receiver, pos / m_voxelSize);

        unlockAllReceiver();
    }
    void removeSyncReceiverMaster(t_receiverIdentifierPtr receiver)
    {
        t_base::removeReceiverMaster(receiver);

        unlockAllReceiver();
    }

    void sendSetTileMaster(t_receiverIdentifierPtr receiver, const t_tileId &id, t_tileDataPtr data)
    {       
        std::stringstream result;
        {
            blub::serialization::format::binary::output format(result);

            sendType typeToSend = sendType::setTile;
            if (data.isNull())
            {
                typeToSend = sendType::removeTile;
            }
            format << typeToSend;
            format << id;
            if (!data.isNull())
            {
                format << *data.get();
            }
        } // flush happens here
        t_tileDataPtr send(new byteArray(result.str().c_str(), result.str().size()));

        m_sigSendTileData(receiver, send);
    }
    void sendLockUnlockForEditMaster(sender::t_receiverIdentifierPtr receiver, const bool& lock)
    {
        std::stringstream result;
        {
            blub::serialization::format::binary::output format(result);

            sendType typeToSend = sendType::unlockForEdit;
            if (lock)
            {
                typeToSend = sendType::lockForEdit;
            }
            format << typeToSend;
        } // flush happens here
        t_tileDataPtr send(new byteArray(result.str().c_str(), result.str().size()));

        m_sigSendTileData(receiver, send);
    }
    void sendLockForEditMaster(sender::t_receiverIdentifierPtr receiver)
    {
        sendLockUnlockForEditMaster(receiver, true);
    }
    void sendUnlockForEditMaster(sender::t_receiverIdentifierPtr receiver)
    {
        sendLockUnlockForEditMaster(receiver, false);
    }

    void compressTileWorker(const t_tileId& id, const t_tileAccessorPtr &tile, t_tileDataPtr toSave)
    {
        BASSERT(!tile.isNull());

        std::stringstream toCompress;
        {
            blub::serialization::format::binary::output format(toCompress);

            format << *tile.get();
        } // flush happens here

        std::ostringstream dataContainerCompressed;
        {
            boost::iostreams::filtering_ostream filterOut;
            filterOut.push(boost::iostreams::bzip2_compressor());
            filterOut.push(dataContainerCompressed);

            boost::iostreams::copy(toCompress, filterOut);
        }

        const int32 sizeCompressed = dataContainerCompressed.str().size();

        if (toSave.isNull())
        {
            toSave = new byteArray(dataContainerCompressed.str().data(), sizeCompressed);
        }
        else
        {
            toSave->resize(sizeCompressed);
            memcpy(toSave->data(), dataContainerCompressed.str().data(), sizeCompressed);
        }

        t_base::m_master.post(boost::bind(&sender::compressTileAfterMaster, this, id, tile, toSave));
    }
    void compressTileAfterMaster(const t_tileId& id, const t_tileAccessorPtr &tile, t_tileDataPtr toSave)
    {
        t_tileDataMap::const_iterator it = m_tileData.find(id);
        const bool found(it != m_tileData.cend());

        if (found && tile.isNull()) // remove tile
        {
            m_tileData.erase(it);
            t_base::removeSyncMaster(id);
        }
        else
        if (found && !tile.isNull()) // change - no save in m_tileData needed, pointer still valid
        {
            typename t_base::t_syncToReceiversMap::const_iterator itTile = t_base::m_syncReceivers.find(id);
            BASSERT(itTile != t_base::m_syncReceivers.cend());
            const typename t_base::t_receiverList& receivers(itTile->second);
            for (typename t_base::t_receiverList::const_iterator itRec = receivers.cbegin(); itRec != receivers.cend(); ++itRec)
            {
                lockReceiver(*itRec);
                sendSetTileMaster(*itRec, id, toSave);
            }
        }
        else
        if (!found && !tile.isNull()) // add
        {
            m_tileData.insert(id, toSave);

            const vector3int32 pos(id*t_base::m_syncTree.getMinNodeSize() + t_base::m_syncTree.getMinNodeSize()/2);
            t_base::addSyncMaster(id, vector3(pos));
        }
        else
        {
			BLUB_SYNC_LOG_ERROR() << "compressTileAfterMaster: invalid case";
        }

        --m_numtilesInWork;
        BASSERT(m_numtilesInWork >= 0);
        if (m_numtilesInWork == 0)
        {
            m_voxels->unlockRead();

            unlockAllReceiver();
        }
    }


    void lockReceiver(const typename t_base::t_receiver toLock)
    {
        if (m_lockedReceiverList.find(toLock) == m_lockedReceiverList.cend())
        {
            m_lockedReceiverList.insert(toLock);
            sendLockForEditMaster(toLock);
        }
    }
    void unlockAllReceiver()
    {
        // blub::BOUT("sender::unlockAllReceiver() m_lockedReceiverList.size():" + blub::string::number(m_lockedReceiverList.size()));
        for (auto rec : m_lockedReceiverList)
        {
            sendUnlockForEditMaster(rec);
        }
        m_lockedReceiverList.clear();
    }

    virtual bool isInSyncRangeReceiver(const typename t_base::t_receiver receiver, const vector3 &posOfReceiverLeafCenter, const typename t_base::t_syncTree::t_nodePtr& octreeNode)
    {
        (void)receiver;
        return m_searchFunction(posOfReceiverLeafCenter, axisAlignedBox(octreeNode->getBoundingBox()));
    }
    virtual bool isInSyncRangeSync(const typename t_base::t_sync sync, const vector3 &posOfSyncLeafCenter, const typename t_base::t_receiverTree::t_nodePtr& octreeNode)
    {
        (void)sync;
        return m_searchFunction(posOfSyncLeafCenter, axisAlignedBox(octreeNode->getBoundingBox()));
    }
    void addSyncReceiver(const typename t_base::t_receiver receiver, const typename t_base::t_sync sync) override
    {
        lockReceiver(receiver);

        t_tileDataMap::const_iterator it(m_tileData.find(sync));
        BASSERT(it != m_tileData.cend());
        sendSetTileMaster(receiver, sync, it->second);
    }
    void removeSyncReceiver(const typename t_base::t_receiver receiver, const typename t_base::t_sync sync) override
    {
        lockReceiver(receiver);

        sendSetTileMaster(receiver, sync, nullptr);
    }



    blub::async::dispatcher &m_worker;

    t_multipleTilesAccessor* m_voxels;
    t_octreeSearchCallback m_searchFunction;
    real m_voxelSize;

    t_tileDataMap m_tileData;

    int32 m_numtilesInWork;
    t_lockedReceiverList m_lockedReceiverList;

    t_sigSendTileData m_sigSendTileData;

    boost::signals2::scoped_connection m_connTileVoxelEditDone;

};


}
}
}
}
}


#endif // NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_SENDER_HPP
