#ifndef NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_SENDER_HPP
#define NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_SENDER_HPP

#include "blub/async/dispatcher.hpp"
#include "blub/async/predecl.hpp"
#include "blub/core/byteArray.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/signal.hpp"
#include "blub/core/vector.hpp"
#include "blub/log/global.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/procedural/predecl.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/sync/voxel/accessor/multipleTiles/base.hpp"
#include "blub/sync/voxel/accessor/multipleTiles/sender.hpp"


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


template <class voxelType, class identifierType>
class sender
{
public:
    typedef sharedPointer<byteArray> t_tileDataPtr;
    typedef identifierType t_identifier;
    typedef multipleTiles::sender<voxelType, t_identifier> t_multipleTiles;
    typedef t_multipleTiles* t_multipleTilesPtr;
    typedef vector<t_multipleTilesPtr> t_multipleTilesList;
    typedef vector<real> t_syncRadiusList;
    typedef sharedPointer<t_identifier> t_receiverIdentifierPtr;

    typedef blub::procedural::voxel::terrain::accessor<voxelType> t_terrainAccessor;
    typedef procedural::voxel::simple::base<sharedPointer<procedural::voxel::tile::accessor<voxelType> > > t_simpleAccessor;

    typedef procedural::voxel::tile::container<voxelType> t_tileContainer;


    sender(async::dispatcher &worker,
           const t_syncRadiusList& syncRadien,
           t_terrainAccessor* tiles)
        : m_syncRadien(syncRadien)
    {
        BASSERT(tiles != nullptr);
        BASSERT(tiles->getNumLod() <= (int32)syncRadien.size());

        real voxelSize(1.);

        for (int32 indLod = 0; indLod < tiles->getNumLod(); ++indLod)
        {
            const auto callback(boost::bind(&sender::isInRange, this, _1, _2, indLod));

            t_simpleAccessor* toWork(tiles->getLod(indLod));
            t_multipleTilesPtr lod(new t_multipleTiles(worker, voxelSize, callback, toWork));
            voxelSize*=2.;
            m_multipleTiles.push_back(lod);

            lod->signalSendTileData()->connect(boost::bind(&sender::lodWantsToSendTileData, this, _1, _2, indLod));
        }
    }
    virtual ~sender()
    {
        for (t_multipleTilesPtr toDelete : m_multipleTiles)
        {
            delete toDelete;
        }
        m_multipleTiles.clear();
    }

    t_multipleTilesPtr getLodSync(const uint16& lod) const
    {
        return m_multipleTiles.at(lod);
    }

    // add/update/remove sync-reveiver
    void addSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        for (t_multipleTilesPtr work : m_multipleTiles)
        {
            work->addSyncReceiver(receiver, pos);
        }
    }
    void updateSyncReceiver(t_receiverIdentifierPtr receiver, const vector3& pos)
    {
        for (t_multipleTilesPtr work : m_multipleTiles)
        {
            work->updateSyncReceiver(receiver, pos);
        }
    }
    void removeSyncReceiver(t_receiverIdentifierPtr receiver)
    {
        for (t_multipleTilesPtr work : m_multipleTiles)
        {
            work->removeSyncReceiver(receiver);
        }
    }

    // to "send sync" signals
    typedef blub::signal<void (t_receiverIdentifierPtr, t_tileDataPtr)> t_sigSendTileData;
    t_sigSendTileData* signalSendTileData()
    {
        return &m_sigSendTileData;
    }

protected:
    void lodWantsToSendTileData(t_receiverIdentifierPtr rec, t_tileDataPtr data, const uint32 &lodInd)
    {
        std::ostringstream result;
        {
            blub::serialization::format::binary::output format(result);

            format << lodInd;
            format << *data.get();
        } // flush happens here
        t_tileDataPtr dataWithLodInformation(new byteArray(result.str().c_str(), result.str().size()));

        m_sigSendTileData(rec, dataWithLodInformation);
    }
    bool isInRange(const vector3& posLeafCenter, const axisAlignedBox& octreeNode, const uint32& lod)
    {
        const vector3 sizeLeaf(t_tileContainer::voxelLength);
        const vector3 sizeLeafDoubled(sizeLeaf*2);
        const vector3 octreeNodeTwiceMinimum((octreeNode.getMinimum()/sizeLeafDoubled).getFloor()*sizeLeafDoubled);
        const axisAlignedBox octreeNodeTwice(octreeNodeTwiceMinimum, octreeNodeTwiceMinimum + octreeNode.getSize()*2.);

        const vector3 posLeafCenterScaled((posLeafCenter / sizeLeafDoubled).getFloor());

        if (lod != 0 && false)
        {
            if (octreeNode.getSize() == sizeLeaf)
            {
                const real radius(m_syncRadien[lod-1] / 2.);
                blub::sphere coll2(posLeafCenter, radius);
                if (coll2.intersects(octreeNode))
                {
                    return false;
                }
            }
        }

        const real radius(m_syncRadien[lod]);
        blub::sphere coll(posLeafCenterScaled * sizeLeafDoubled + sizeLeaf, radius);
        const bool result(coll.intersects(octreeNodeTwice));

        return result;
    }

protected:
    t_syncRadiusList m_syncRadien;
    t_multipleTilesList m_multipleTiles;

    t_sigSendTileData m_sigSendTileData;

};


}
}
}
}
}


#endif // NETWORK_SYNC_VOXEL_ACCESSOR_TERRAIN_SENDER_HPP
