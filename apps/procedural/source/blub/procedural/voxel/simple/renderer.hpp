#ifndef VOXEL_SIMPLE_RENDERER_HPP
#define VOXEL_SIMPLE_RENDERER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/math/vector3.hpp"
#include "blub/procedural/voxel/simple/base.hpp"
#include "blub/sync/predecl.hpp"
#include "blub/sync/sender.hpp"

#include <boost/function/function2.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


class renderer
        : public base<sharedPointer<tile::renderer> >
{
public:
    typedef vector3int32 t_tileId;
    typedef sharedPointer<tile::renderer> t_tilePtr;

    typedef sharedPointer<sync::identifier> t_cameraPtr;
    typedef sync::sender<t_tileId, t_cameraPtr> t_sync;
    typedef base<t_tilePtr> t_base;

    typedef hashMap<vector3int32, t_tilePtr> t_tileMap;

    typedef sharedPointer<tile::surface> t_tileDataPtr;

    typedef boost::function<bool (vector3, axisAlignedBox)> t_octreeSearchCallback;
    typedef boost::function<t_tilePtr (int32 lod, vector3int32 id)> t_createTileCallback;

    typedef base<sharedPointer<tile::surface> > t_rendererSurface;


    renderer(blub::async::dispatcher &worker,
             t_rendererSurface *tiles,
             const t_createTileCallback &createTileCallback,
             const int32 &lod,
             const real &voxelSize,
             const real &lodCutDistNear,
             const real &lodCutDistFar);
    virtual ~renderer();

    // adding more than one camera is untested
    void addCamera(t_cameraPtr toAdd, const blub::vector3& position);
    void updateCamera(t_cameraPtr toUpdate, const blub::vector3& position);
    void removeCamera(t_cameraPtr toRemove);

    void editDone();

protected:
    void editDoneMaster();

    void tileGotSetMaster(const t_tileId& id, const t_tileDataPtr toSet);
    void tileGotRemovedMaster(const t_tileId& id);

    bool isInSyncRangeReceiver(const t_sync::t_receiver receiver,
                               const vector3 &posOfReceiverLeafCenter,
                               const typename t_sync::t_syncTree::t_nodePtr& octreeNode);
    bool isInSyncRangeSync(const t_sync::t_sync sync,
                           const vector3 &posOfSyncLeafCenter,
                           const typename t_sync::t_receiverTree::t_nodePtr& octreeNode);

    void addSyncReceiver(const t_sync::t_receiver receiver,
                         const t_sync::t_sync sync);
    void removeSyncReceiver(const t_sync::t_receiver receiver,
                            const t_sync::t_sync sync);

    void updateLod(const t_tileId &id, t_tilePtr toUpdate);

    int32 isInRange(const vector3& posLeafCenter, const axisAlignedBox& octreeNode);
    void updateCameraMaster(t_cameraPtr toUpdate, const blub::vector3& position);

private:
    t_createTileCallback m_createTileCallback;
    const int32 m_lod;
    const real m_lodCutDistNear;
    const real m_lodCutDistFar;
    vector3 m_cameraPositionInTreeLeaf;
    real m_voxelSize;
    t_rendererSurface* m_voxels;

    t_tileMap m_tileData;
    t_sync *m_sync;

};


}
}
}
}


#endif // VOXEL_SIMPLE_RENDERER_HPP
