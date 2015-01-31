#ifndef SYNC_SENDER_HPP
#define SYNC_SENDER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/signal.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/math/octree/search.hpp"
#include "blub/async/strand.hpp"

#include "boost/function/function2.hpp"
#include "boost/function/function3.hpp"


namespace blub
{
namespace sync
{


template <typename syncType, typename receiverType>
class sender
{
public:
    typedef syncType t_sync;
    typedef receiverType t_receiver;

    typedef octree::container<t_sync> t_syncTree;
    typedef octree::container<t_receiver> t_receiverTree;

    typedef hashMap<t_receiver, vector3> t_receiverPosMap;
    typedef hashMap<t_sync, vector3> t_syncPosMap;

    typedef hashList<t_sync> t_syncList;
    typedef hashList<t_receiver> t_receiverList;

    typedef hashMap<t_receiver, t_syncList> t_receiverToSyncsMap;
    typedef hashMap<t_sync, t_receiverList> t_syncToReceiversMap;

    // callbacks
    typedef boost::function<bool (t_receiver, vector3, typename t_syncTree::t_nodePtr)> t_callbackInSyncRangeReceiver;
    typedef boost::function<bool (t_sync, vector3, typename t_receiverTree::t_nodePtr)> t_callbackInSyncRangeSync;


    sender(blub::async::dispatcher &worker, const vector3int32& treeSize)
        : m_master(worker)
        , m_syncTree(treeSize)
        , m_receiverTree(treeSize)
    {

    }
    virtual ~sender()
    {
        ;
    }

    // to "send sync"
    void addSync(const t_sync toSync, const vector3& pos)
    {
         m_master.dispatch(boost::bind(&sender::addSyncMaster, this, toSync, pos));
    }
    void updateSync(const t_sync toSync, const vector3& pos)
    {
        m_master.dispatch(boost::bind(&sender::updateSyncMaster, this, toSync, pos));
    }
    void removeSync(const t_sync toSync)
    {
        m_master.dispatch(boost::bind(&sender::removeSyncMaster, this, toSync));
    }

    // add/update/remove sync-reveiver
    void addReceiver(t_receiver receiver, const vector3& pos)
    {
        m_master.dispatch(boost::bind(&sender::addReceiverMaster, this, receiver, pos));
    }
    void updateReceiver(t_receiver receiver, const vector3& pos)
    {
        m_master.dispatch(boost::bind(&sender::updateReceiverMaster, this, receiver, pos));
    }
    void removeReceiver(t_receiver receiver)
    {
        m_master.dispatch(boost::bind(&sender::removeReceiverMaster, this, receiver));
    }

    void addSyncMaster(const t_sync toSync, const vector3& pos)
    {
        BASSERT(m_syncPosMap.find(toSync) == m_syncPosMap.cend());
        BASSERT(m_syncReceivers.find(toSync) == m_syncReceivers.cend());
        BASSERT(m_syncTree.getNodes(toSync).empty());

        m_syncPosMap.insert(toSync, pos);

        m_syncReceivers.insert(toSync, t_receiverList());

#ifdef BLUB_DEBUG
        bool result =
#endif
                m_syncTree.insert(toSync, pos);
        BASSERT(result);

        updateLinkSyncReceiverMaster(toSync);
    }
    void updateSyncMaster(const t_sync toSync, const vector3& pos)
    {
        BASSERT(m_syncPosMap.find(toSync) != m_syncPosMap.cend());
        BASSERT(m_syncReceivers.find(toSync) != m_syncReceivers.cend());
        BASSERT(!m_syncTree.getNodes(toSync).empty());

        m_syncPosMap.insert(toSync, pos);

        if (!m_syncTree.update(toSync, pos))
        {
            // nothing changed
            return;
        }

        updateLinkSyncReceiverMaster(toSync);
    }
    void removeSyncMaster(const t_sync toSync)
    {
        BASSERT(m_syncPosMap.find(toSync) != m_syncPosMap.cend());
        BASSERT(m_syncReceivers.find(toSync) != m_syncReceivers.cend());
        BASSERT(!m_syncTree.getNodes(toSync).empty());

        {
            m_syncPosMap.erase(m_syncPosMap.find(toSync));
        }

        {
#ifdef BLUB_DEBUG
            bool result =
#endif
                     m_syncTree.remove(toSync);
            BASSERT(result);
        }

        {
            typename t_syncToReceiversMap::iterator it = m_syncReceivers.find(toSync);
            BASSERT(it != m_syncReceivers.cend());
            const t_receiverList& accordingReceivers(it->second);
            while (!accordingReceivers.empty())
            {
                removeLinkSyncReceiverMaster(*accordingReceivers.cbegin(), toSync);
            }
            m_syncReceivers.erase(it);
        }
    }

    void addReceiverMaster(t_receiver receiver, const vector3& pos)
    {
        BASSERT(m_receiverPosMap.find(receiver) == m_receiverPosMap.cend());
        BASSERT(m_receiverSyncs.find(receiver) == m_receiverSyncs.cend());
        BASSERT(m_receiverTree.getNodes(receiver).empty());

        m_receiverPosMap.insert(receiver, pos);

        m_receiverSyncs.insert(receiver, t_syncList());

#ifdef BLUB_DEBUG
        bool result =
#endif
                m_receiverTree.insert(receiver, pos);
        BASSERT(result);

        updateLinkReceiverSyncMaster(receiver);
    }
    void updateReceiverMaster(t_receiver receiver, const vector3& pos)
    {
        BASSERT(m_receiverPosMap.find(receiver) != m_receiverPosMap.cend());
        BASSERT(m_receiverSyncs.find(receiver) != m_receiverSyncs.cend());
        BASSERT(!m_receiverTree.getNodes(receiver).empty());

        m_receiverPosMap.insert(receiver, pos);

        if (!m_receiverTree.update(receiver, pos))
        {
            // no update in tree, nothing to resync
            // nothing changed
            return;
        }

        updateLinkReceiverSyncMaster(receiver);
    }
    void removeReceiverMaster(t_receiver receiver)
    {
        BASSERT(m_receiverPosMap.find(receiver) != m_receiverPosMap.cend());
        BASSERT(m_receiverSyncs.find(receiver) != m_receiverSyncs.cend());
        BASSERT(!m_receiverTree.getNodes(receiver).empty());

        {
            m_receiverPosMap.erase(m_receiverPosMap.find(receiver));
        }

        {
#ifdef BLUB_DEBUG
        bool result =
#endif
                m_receiverTree.remove(receiver);
            BASSERT(result);
        }

        {
            typename t_receiverToSyncsMap::iterator it = m_receiverSyncs.find(receiver);
            BASSERT(it != m_receiverSyncs.cend());
            const t_syncList& accordingSyncs(it->second);
            while (!accordingSyncs.empty())
            {
                removeLinkSyncReceiverMaster(receiver, *accordingSyncs.cbegin());
            }
            m_receiverSyncs.erase(it);
        }
    }

    blub::async::strand &getMaster()
    {
        return m_master;
    }

    void setCallbackInSyncRangeReceiver(t_callbackInSyncRangeReceiver toSet)
    {
        m_callbackInSyncRangeReceiver = toSet;
    }
    void setCallbackInSyncRangeSync(t_callbackInSyncRangeSync toSet)
    {
        m_callbackInSyncRangeSync = toSet;
    }

    typedef blub::signal<void (t_receiver, t_sync)> t_sigAdd;
    t_sigAdd* signalAdd()
    {
        return &m_sigAdd;
    }
    typedef blub::signal<void (t_receiver, t_sync)> t_sigRemove;
    t_sigRemove* signalRemove()
    {
        return &m_sigRemove;
    }

protected:
    void updateLinkReceiverSyncMaster(t_receiver receiver)
    {
        auto leafs(m_receiverTree.getNodes(receiver));
        BASSERT(leafs.size() == 1);
        auto leaf(*leafs.begin());
        vector3int32 centerLeaf(leaf->getPosition()+m_receiverTree.getMinNodeSize()/2);

        auto callbackForOctree(boost::bind(&sender<t_sync, t_receiver>::isInSyncRangeReceiver, this, receiver, vector3(centerLeaf.x, centerLeaf.y, centerLeaf.z), _1));
        const typename octree::search<t_sync>::t_dataList result(octree::search<t_sync>::getDataByUserDefinedFunction(m_syncTree, callbackForOctree));

        typename t_receiverToSyncsMap::const_iterator it = m_receiverSyncs.find(receiver);
        BASSERT(it != m_receiverSyncs.cend());

        // check for removed syncs
        const t_syncList syncs(it->second);
        {
            for (t_sync sync : syncs)
            {
                if (result.find(sync) == result.cend())
                {
                    removeLinkSyncReceiverMaster(receiver, sync);
                }
            }
        }
        // check for add sync
        {
            for (t_sync sync : result)
            {
                if (syncs.find(sync) == syncs.cend())
                {
                    addLinkSyncReceiverMaster(receiver, sync);
                }
            }
        }
    }

    void updateLinkSyncReceiverMaster(t_sync sync)
    {
        if (m_receiverPosMap.empty()) // nothing to sync
        {
            return;
        }

        auto leafs(m_syncTree.getNodes(sync));
        BASSERT(leafs.size() == 1);
        auto leaf(*leafs.begin());
        vector3int32 centerLeaf(leaf->getPosition()+m_receiverTree.getMinNodeSize()/2);

        auto callbackForOctree(boost::bind(&sender<t_sync, t_receiver>::isInSyncRangeSync, this, sync, vector3(centerLeaf.x, centerLeaf.y, centerLeaf.z), _1));
        const typename octree::search<t_receiver>::t_dataList result(octree::search<t_receiver>::getDataByUserDefinedFunction(m_receiverTree, callbackForOctree));

        typename t_syncToReceiversMap::const_iterator it = m_syncReceivers.find(sync);
        BASSERT(it != m_syncReceivers.cend());

        // check for removed syncs
        const t_receiverList receivers(it->second);
        {
            for (t_receiver receiver : receivers)
            {
                if (result.find(receiver) == result.cend())
                {
                    removeLinkSyncReceiverMaster(receiver, sync);
                }
            }
        }
        // check for add sync
        {
            for (t_receiver receiver : result)
            {
                if (receivers.find(receiver) == receivers.cend())
                {
                    addLinkSyncReceiverMaster(receiver, sync);
                }
            }
        }
    }

    void addLinkSyncReceiverMaster(const t_receiver receiver, const t_sync sync)
    {
        {
            typename t_syncToReceiversMap::iterator itSync = m_syncReceivers.find(sync);
            BASSERT(itSync != m_syncReceivers.cend());
            BASSERT(itSync->second.find(receiver) == itSync->second.cend());
            itSync->second.insert(receiver);
        }
        {
            typename t_receiverToSyncsMap::iterator itReceiver = m_receiverSyncs.find(receiver);
            BASSERT(itReceiver != m_receiverSyncs.cend());
            BASSERT(itReceiver->second.find(sync) == itReceiver->second.cend());
            itReceiver->second.insert(sync);
        }
        addSyncReceiver(receiver, sync);
    }
    void removeLinkSyncReceiverMaster(const t_receiver receiver, const t_sync sync)
    {
        {
            typename t_syncToReceiversMap::iterator itSync = m_syncReceivers.find(sync);
            BASSERT(itSync != m_syncReceivers.cend());
            typename  t_receiverList::const_iterator itReceiver = itSync->second.find(receiver);
            BASSERT(itReceiver != itSync->second.cend());
            itSync->second.erase(itReceiver);
        }
        {
            typename t_receiverToSyncsMap::iterator itReceiver = m_receiverSyncs.find(receiver);
            BASSERT(itReceiver != m_receiverSyncs.cend());
            typename t_syncList::const_iterator itSync = itReceiver->second.find(sync);
            BASSERT(itSync != itReceiver->second.cend());
            itReceiver->second.erase(itSync);
        }
        removeSyncReceiver(receiver, sync);
    }

    virtual bool isInSyncRangeReceiver(const t_receiver receiver, const vector3& posOfReceiverLeafCenter, const typename t_syncTree::t_nodePtr& octreeNode)
    {
        return m_callbackInSyncRangeReceiver(receiver, posOfReceiverLeafCenter, octreeNode);
    }

    virtual bool isInSyncRangeSync(const t_sync sync, const vector3& posOfSyncLeafCenter, const typename t_receiverTree::t_nodePtr& octreeNode)
    {
        return m_callbackInSyncRangeSync(sync, posOfSyncLeafCenter, octreeNode);
    }

    virtual void addSyncReceiver(const t_receiver receiver, const t_sync sync)
    {
        m_sigAdd(receiver, sync);
    }

    virtual void removeSyncReceiver(const t_receiver receiver, const t_sync sync)
    {
        m_sigRemove(receiver, sync);
    }

protected:
    async::strand m_master;

    t_syncTree m_syncTree;
    t_syncToReceiversMap m_syncReceivers;
    t_syncPosMap m_syncPosMap;

    t_receiverTree m_receiverTree;
    t_receiverToSyncsMap m_receiverSyncs;
    t_receiverPosMap m_receiverPosMap;

    t_callbackInSyncRangeReceiver m_callbackInSyncRangeReceiver;
    t_callbackInSyncRangeSync m_callbackInSyncRangeSync;

    t_sigAdd m_sigAdd;
    t_sigRemove m_sigRemove;

};



}
}


#endif // SYNC_SENDER_HPP
