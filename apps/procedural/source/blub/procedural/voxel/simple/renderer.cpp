#include "renderer.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/core/log.hpp"
#include "blub/math/octree/search.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/simple/surface.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"

#include <boost/function/function2.hpp>


using namespace blub::procedural::voxel::simple;
using namespace blub;


renderer::renderer(blub::async::dispatcher &worker,
                   t_rendererSurface *tiles,
                   const t_createTileCallback &createTileCallback,
                   const int32& lod,
                   const real& voxelSize,
                   const real& lodCutDistNear,
                   const real& lodCutDistFar)
    : t_base(worker)
    , m_createTileCallback(createTileCallback)
    , m_lod(lod)
    , m_lodCutDistNear(lodCutDistNear)
    , m_lodCutDistFar(lodCutDistFar)
    , m_voxelSize(voxelSize)
    , m_voxels(tiles)
{
    m_sync = new t_sync(worker, vector3int32(procedural::voxel::tile::container::voxelLength));
    m_sync->setCallbackInSyncRangeReceiver(boost::bind(&renderer::isInSyncRangeReceiver, this, _1, _2, _3));
    m_sync->setCallbackInSyncRangeSync(boost::bind(&renderer::isInSyncRangeSync, this, _1, _2, _3));
    m_sync->signalAdd()->connect(boost::bind(&renderer::addSyncReceiver, this, _1, _2));
    m_sync->signalRemove()->connect(boost::bind(&renderer::removeSyncReceiver, this, _1, _2));

    m_voxels->signalEditDone()->connect(boost::bind(&renderer::editDone, this));
}

renderer::~renderer()
{
    delete m_sync;
}

void renderer::addCamera(renderer::t_cameraPtr toAdd, const vector3 &position)
{
    m_sync->addReceiver(toAdd, position / m_voxelSize);
}

void renderer::updateCamera(renderer::t_cameraPtr toUpdate, const vector3 &position)
{
    m_sync->getMaster().dispatch(boost::bind(&renderer::updateCameraMaster, this, toUpdate, position));
}

void renderer::removeCamera(renderer::t_cameraPtr toRemove)
{
    m_sync->removeReceiver(toRemove);
}


void renderer::editDone()
{
    m_voxels->lockForRead();
    m_worker.post(boost::bind(&renderer::editDoneMaster, this));
}

void renderer::editDoneMaster()
{
    auto& change(m_voxels->getTilesThatGotEdited());
    for (auto hasChanged : change)
    {
        t_tileId id(hasChanged.first);
        t_tileDataPtr work(hasChanged.second);

        if (work.isNull())
        {
            tileGotRemovedMaster(id);
        }
        else
        {
            tileGotSetMaster(id, work);
        }
    }

    m_voxels->unlockRead();
}

void renderer::tileGotSetMaster(const renderer::t_tileId &id, const renderer::t_tileDataPtr toSet)
{
    BASSERT(toSet->getIndices().size() > 0);

    t_tileMap::const_iterator it = m_tileData.find(id);
    const bool found(it != m_tileData.cend());

    axisAlignedBox aabb(vector3(id*procedural::voxel::tile::container::voxelLength),
                        vector3(id*procedural::voxel::tile::container::voxelLength+vector3int32(procedural::voxel::tile::container::voxelLength+1)));
    aabb*=m_voxelSize;
    t_tilePtr workTile;
    if (found)
    {
        workTile = it->second;
    }
    else
    {
        workTile = m_createTileCallback(m_lod, id);
    }

    workTile->setTileData(toSet.data(), aabb);

    if (!found)
    {
        m_tileData.insert(id, workTile);

        const vector3int32 size(procedural::voxel::tile::container::voxelLength);
        vector3int32 pos(id*size + size/2);
        m_sync->addSyncMaster(id, vector3(pos));
    }
}

void renderer::tileGotRemovedMaster(const renderer::t_tileId &id)
{
    t_tileMap::const_iterator it = m_tileData.find(id);
    BASSERT(it != m_tileData.cend());

    m_sync->removeSyncMaster(id);

    m_tileData.erase_return_void(it);
}

bool renderer::isInSyncRangeReceiver(const t_sync::t_receiver receiver,
                                          const vector3 &posOfReceiverLeafCenter,
                                          const typename t_sync::t_syncTree::t_nodePtr& octreeNode)
{
    (void)receiver;
    return isInRange(posOfReceiverLeafCenter, axisAlignedBox(octreeNode->getBoundingBox())) == 0;
}

bool renderer::isInSyncRangeSync(const t_sync::t_sync sync,
                                      const vector3 &posOfSyncLeafCenter,
                                      const typename t_sync::t_receiverTree::t_nodePtr& octreeNode)
{
    (void)sync;
    return isInRange(posOfSyncLeafCenter, axisAlignedBox(octreeNode->getBoundingBox())) == 0;
}

void renderer::addSyncReceiver(const t_sync::t_receiver receiver,
                                    const t_sync::t_sync sync)
{
    (void)receiver;
    t_tileMap::const_iterator it(m_tileData.find(sync));
    BASSERT(it != m_tileData.cend());

    it->second->setVisible(true);

    updateLod(sync, it->second);
}

void renderer::removeSyncReceiver(const t_sync::t_receiver receiver,
                                       const t_sync::t_sync sync)
{
    (void)receiver;
    t_tileMap::const_iterator it(m_tileData.find(sync));
    BASSERT(it != m_tileData.cend());

    it->second->setVisible(false);

    updateLod(sync, it->second);
}

void renderer::updateLod(const renderer::t_tileId &id, t_tilePtr toUpdate)
{
    if (m_lod == 0)
    {
        return;
    }
    /*if (toUpdate->getVisible() && isInRange(m_cameraPositionInTreeLeaf, axisAlignedBox(neighbourOctreeNode)) == 2)
    {
        return; // got invis because too far away --> no lod on this tile
        // leads to bug when syncRadien are chosen too small.
    }*/

    const int32 sizeLeaf(procedural::voxel::tile::container::voxelLength);
    const vector3int32 posAbs(id*sizeLeaf);
    const axisAlignedBoxInt32 octreeNode(posAbs, posAbs+vector3int32(sizeLeaf));
    const vector3int32 toIterate[] = {{-1, 0, 0},
                                      {1, 0, 0},
                                      {0, -1, 0},
                                      {0, 1, 0},
                                      {0, 0, -1},
                                      {0, 0, 1}
                                     };
    const int32 toSetOnNeighbour[] = {1, 0, 3, 2, 5, 4};
    const int32 tileWork(isInRange(m_cameraPositionInTreeLeaf, axisAlignedBox(octreeNode)));
    if ((tileWork == 0) != toUpdate->getVisible())
    {
        blub::BWARNING("(tileWork == 0) != toUpdate->getVisible() id:" + blub::string::number(id)); // may occur, when server deletes tile message, arrives before up-to-date camera position.
    }
    for (int32 lod = 0; lod < 6; ++lod)
    {
        const vector3int32 neighbourId(id+toIterate[lod]);
        const vector3int32 neighbourPosAbs(neighbourId*sizeLeaf);
        const axisAlignedBoxInt32 neighbourOctreeNode(neighbourPosAbs, neighbourPosAbs+vector3int32(sizeLeaf));
        const int32 doLod(isInRange(m_cameraPositionInTreeLeaf, axisAlignedBox(neighbourOctreeNode)));
        if (toUpdate->getVisible())
        {
            toUpdate->setVisibleLod(lod, doLod == 1);
        }
        else
        {
            toUpdate->setVisibleLod(lod, false);
        }
        t_tileMap::const_iterator it(m_tileData.find(neighbourId));
        if (it == m_tileData.cend())
        {
            continue;
        }
        if (toUpdate->getVisible())
        {
            it->second->setVisibleLod(toSetOnNeighbour[lod], false);
        }
        else
        {
            it->second->setVisibleLod(toSetOnNeighbour[lod], tileWork == 1);
        }
    }
}

int32 renderer::isInRange(const vector3& posLeafCenter, const axisAlignedBox& octreeNode)
{
    const vector3 sizeLeaf(procedural::voxel::tile::container::voxelLength);
    const vector3 sizeLeafDoubled(sizeLeaf*2);
    const vector3 octreeNodeTwiceMinimum((octreeNode.getMinimum()/sizeLeafDoubled).getFloor()*sizeLeafDoubled);
    const axisAlignedBox octreeNodeTwice(octreeNodeTwiceMinimum, octreeNodeTwiceMinimum + octreeNode.getSize()*2.);

    const vector3 posLeafCenterScaled((posLeafCenter / sizeLeafDoubled).getFloor());

    if (m_lod != 0)
    {
        if (octreeNode.getSize() == sizeLeaf)
        {
            const real radius(m_lodCutDistNear / 2.);
            blub::sphere coll2(posLeafCenter, radius);
            if (coll2.intersects(octreeNode))
            {
                return 1;
            }
        }
    }

    const real radius(m_lodCutDistFar);
    blub::sphere coll(posLeafCenterScaled * sizeLeafDoubled + sizeLeaf, radius);
    if(coll.intersects(octreeNodeTwice))
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

void renderer::updateCameraMaster(renderer::t_cameraPtr toUpdate, const vector3 &position)
{
    const vector3 camPosScaled(position / m_voxelSize);
    const real tileContainerSize(procedural::voxel::tile::container::voxelLength);
    m_cameraPositionInTreeLeaf = (camPosScaled/tileContainerSize).getFloor()*tileContainerSize + vector3(tileContainerSize*0.5);
    m_sync->updateReceiverMaster(toUpdate, camPosScaled);
}
