#ifndef BLUB_PROCEDURAL_VOXEL_SIMPLE_RENDERER_HPP
#define BLUB_PROCEDURAL_VOXEL_SIMPLE_RENDERER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/log.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/math/vector3.hpp"
#include "blub/procedural/voxel/simple/base.hpp"
#include "blub/procedural/voxel/simple/surface.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"
#include "blub/sync/predecl.hpp"
#include "blub/sync/sender.hpp"

#include <boost/function/function0.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


/**
 * @brief The renderer class handles the correct rendering of a lod.
 * Including renderdistance and enabling the submeshes for losing the cracks (transvoxel results).
 * Takes the results and updates from the simple::surface and saves them into an octree.
 * Casts signals on when to update an LOD.
 */
// TODO reimplement class, with better threading and better octree/sync.
template <class voxelType>
class renderer
        : public base<sharedPointer<tile::renderer<voxelType> > >
{
public:
    typedef tile::renderer<voxelType> t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;
    typedef base<t_tilePtr> t_base;
    typedef typename t_base::t_tileId t_tileId;

    typedef tile::container<voxelType> t_tileContainer;

    typedef sharedPointer<sync::identifier> t_cameraPtr;
    typedef sync::sender<t_tileId, t_cameraPtr> t_sync;

    typedef hashMap<vector3int32, t_tilePtr> t_tileMap;

    typedef tile::surface<voxelType> t_tileSurface;
    typedef sharedPointer<t_tileSurface> t_tileDataPtr;

    typedef boost::function<bool (vector3, axisAlignedBox)> t_octreeSearchCallback;

    typedef base<t_tileDataPtr> t_rendererSurface;


    /**
     * @brief renderer constructor.
     * @param worker May get called by several threads.
     * @param tiles Must not be nullptr.
     * @param lod Level of detail index. Zero is the detail with highest resolution, one has half resolution and so on.
     * @param lodCutDistNear Defines distance when a tile when it is too near. Will get disabled if tile is nearer lodCutDistNear.
     * @param lodCutDistFar Defines distance when a tile when it is too far. Will get disabled if tile is farer away than lodCutDistFar.
     */
    renderer(blub::async::dispatcher &worker,
             t_rendererSurface *tiles,
             const int32 &lod,
             const real &lodCutDistNear,
             const real &lodCutDistFar)
        : t_base(worker)
        , m_lod(lod)
        , m_lodCutDistNear(lodCutDistNear)
        , m_lodCutDistFar(lodCutDistFar)
        , m_voxelSize(math::pow(2., m_lod))
        , m_voxels(tiles)
    {
        m_sync = new t_sync(worker, vector3int32(t_tileContainer::voxelLength));
        m_sync->setCallbackInSyncRangeReceiver(boost::bind(&renderer::isInSyncRangeReceiver, this, _1, _2, _3));
        m_sync->setCallbackInSyncRangeSync(boost::bind(&renderer::isInSyncRangeSync, this, _1, _2, _3));
        m_sync->signalAdd()->connect(boost::bind(&renderer::addSyncReceiver, this, _1, _2));
        m_sync->signalRemove()->connect(boost::bind(&renderer::removeSyncReceiver, this, _1, _2));

        m_voxels->signalEditDone()->connect(boost::bind(&renderer::editDone, this));
    }
    /**
     * @brief ~renderer destructor.
     */
    virtual ~renderer()
    {
        delete m_sync;
    }

    /**
     * @brief addCamera adds an camera.
     * @param toAdd Must not be nullptr
     * @param position The initial position of the camera.
     */
    // adding more than one camera is untested - but should work.
    void addCamera(t_cameraPtr toAdd, const blub::vector3& position)
    {
        m_sync->addReceiver(toAdd, position / m_voxelSize);
    }
    /**
     * @brief updateCamera updates the position of a camera you have to add before by using addCamera()
     * @param toUpdate The camera, must not be nullptr.
     * @param position The new position.
     */
    void updateCamera(t_cameraPtr toUpdate, const blub::vector3& position)
    {
        m_sync->getMaster().dispatch(boost::bind(&renderer::updateCameraMaster, this, toUpdate, position));
    }
    /**
     * @brief removeCamera removes a camera.
     * @param toRemove Must not be nullptr.
     */
    void removeCamera(t_cameraPtr toRemove)
    {
        m_sync->removeReceiver(toRemove);
    }

    // lock class for read before work
    /**
     * @brief getTileMap Returns all surface-tiles holded by this class. Read-lock class before.
     * @return
     */
    const t_tileMap &getTileMap() const
    {
        return m_tileData;
    }

protected:
    /**
     * @brief editDone gets called when simple::surface changed. Read-locks surface.
     */
    void editDone()
    {
        m_voxels->lockForRead();
        t_base::m_worker.post(boost::bind(&renderer::editDoneMaster, this));
    }

    /**
     * @brief editDoneMaster gets called when simple::surface changed.
     * @see editDone()
     */
    void editDoneMaster()
    {
        auto& change(m_voxels->getTilesThatGotEdited());
        for (auto hasChanged : change)
        {
            const t_tileId id(hasChanged.first);
            const t_tileDataPtr work(hasChanged.second);

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

    /**
     * @brief tileGotSetMaster sets the surface tiles to the octree.
     * @param id TileId
     * @param toSet The Surface-tile to work on.
     */
    void tileGotSetMaster(const t_tileId& id, const t_tileDataPtr toSet)
    {
        BASSERT(toSet->getIndices().size() > 0);

        typename t_tileMap::const_iterator it = m_tileData.find(id);
        const bool found(it != m_tileData.cend());

        axisAlignedBox aabb(vector3(id*t_tileContainer::voxelLength),
                            vector3(id*t_tileContainer::voxelLength+vector3int32(t_tileContainer::voxelLength)));
        aabb*=m_voxelSize;
        t_tilePtr workTile;
        if (found)
        {
            workTile = it->second;
        }
        else
        {
            workTile = t_base::createTile();
        }

        workTile->setTileData(toSet, aabb);

        if (!found)
        {
            m_tileData.insert(id, workTile);

            const vector3int32 size(t_tileContainer::voxelLength);
            vector3int32 pos(id*size + size/2);
            m_sync->addSyncMaster(id, vector3(pos));
        }
    }

    /**
     * @brief tileGotRemovedMaster removes tile from octree.
     * @param id TileId
     */
    void tileGotRemovedMaster(const t_tileId& id)
    {
        typename t_tileMap::const_iterator it = m_tileData.find(id);
        BASSERT(it != m_tileData.cend());

        m_sync->removeSyncMaster(id);

        m_tileData.erase_return_void(it);
    }

    /**
     * @see isInRange()
     */
    bool isInSyncRangeReceiver(const typename t_sync::t_receiver receiver,
                               const vector3 &posOfReceiverLeafCenter,
                               const typename t_sync::t_syncTree::t_nodePtr& octreeNode)
    {
        (void)receiver;
        return isInRange(posOfReceiverLeafCenter, axisAlignedBox(octreeNode->getBoundingBox())) == 0;
    }
    /**
     * @see isInRange()
     */
    bool isInSyncRangeSync(const typename t_sync::t_sync sync,
                           const vector3 &posOfSyncLeafCenter,
                           const typename t_sync::t_receiverTree::t_nodePtr& octreeNode)
    {
        (void)sync;
        return isInRange(posOfSyncLeafCenter, axisAlignedBox(octreeNode->getBoundingBox())) == 0;
    }

    /**
     * @brief addSyncReceiver gets called by octree if a tile should be visible.
     * @param receiver Camera
     * @param sync Tile
     */
    void addSyncReceiver(const typename t_sync::t_receiver receiver,
                         const typename t_sync::t_sync sync)
    {
        (void)receiver;
        typename t_tileMap::const_iterator it(m_tileData.find(sync));
        BASSERT(it != m_tileData.cend());

        it->second->setVisible(true);

        updateLod(sync, it->second);
    }
    /**
     * @brief removeSyncReceiver gets called by octree if a tile shouldn't be visible.
     * @param receiver Camera
     * @param sync Tile
     */
    void removeSyncReceiver(const typename t_sync::t_receiver receiver,
                            const typename t_sync::t_sync sync)
    {
        (void)receiver;
        typename t_tileMap::const_iterator it(m_tileData.find(sync));
        BASSERT(it != m_tileData.cend());

        it->second->setVisible(false);

        updateLod(sync, it->second);
    }

    /**
     * @brief updateLod checks if a neighbour of the tile has a different lod.
     * If so it enables the by the transvoxel calculated submeshes, used to close the cracks.
     * @param id TileId
     * @param toUpdate Surface-tile to work on.
     */
    void updateLod(const t_tileId &id, t_tilePtr toUpdate)
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

        const int32 sizeLeaf(t_tileContainer::voxelLength);
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
            typename t_tileMap::const_iterator it(m_tileData.find(neighbourId));
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

    /**
     * @brief isInRange checks the distance of a tile to the camera.
     * @param posLeafCenter camera position.
     * @param octreeNode axisAlignedBox of the surface-tile.
     * @return Returns 0 for too near, 2 for too far and 1 for in range.
     */
    int32 isInRange(const vector3& posLeafCenter, const axisAlignedBox& octreeNode)
    {
        const vector3 sizeLeaf(t_tileContainer::voxelLength);
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

    /**
     * @see updateCamera
     */
    void updateCameraMaster(t_cameraPtr toUpdate, const blub::vector3& position)
    {
        const vector3 camPosScaled(position / m_voxelSize);
        const real tileContainerSize(t_tileContainer::voxelLength);
        m_cameraPositionInTreeLeaf = (camPosScaled/tileContainerSize).getFloor()*tileContainerSize + vector3(tileContainerSize*0.5);
        m_sync->updateReceiverMaster(toUpdate, camPosScaled);
    }

private:
    const int32 m_lod;
    const real m_lodCutDistNear;
    const real m_lodCutDistFar;
    vector3 m_cameraPositionInTreeLeaf;
    real m_voxelSize;
    t_rendererSurface* m_voxels;

    t_tileMap m_tileData; // TODO remove me. insert the tile into the tree, instead of the id --> faster and cleaner.
    t_sync *m_sync;

};


}
}
}
}


#endif // BLUB_PROCEDURAL_VOXEL_SIMPLE_RENDERER_HPP
