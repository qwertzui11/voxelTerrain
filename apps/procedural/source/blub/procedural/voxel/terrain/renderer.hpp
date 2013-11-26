#ifndef VOXEL_TERRAIN_RENDERER_HPP
#define VOXEL_TERRAIN_RENDERER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"

#include <boost/function/function_fwd.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{



class renderer : public base<sharedPointer<tile::renderer> >
{
public:
    typedef vector3int32 t_tileId;
    typedef sharedPointer<tile::renderer> t_tilePtr;
    typedef sharedPointer<sync::identifier> t_cameraPtr;

    typedef base<t_tilePtr> t_base;

    typedef procedural::voxel::simple::container::base t_simpleData;
    typedef t_simpleData* t_multipleTilesDataPtr;
    typedef sharedPointer<procedural::voxel::tile::surface> t_tileDataPtr;
    typedef vector<real> t_syncRadiusList;

    typedef simple::renderer t_simple;
    typedef base<sharedPointer<tile::surface> > t_rendererSurface;

    typedef boost::function<t_tilePtr (int32 lod, vector3int32 id)> t_createTileCallback;

    renderer(blub::async::dispatcher &worker,
             t_rendererSurface &renderer_,
             const t_createTileCallback &createTileCallback,
             const t_syncRadiusList& syncRadien,
             const real &voxelSize);
    virtual ~renderer();

    void addCamera(t_cameraPtr toAdd, const blub::vector3& position);
    void updateCamera(t_cameraPtr toUpdate, const blub::vector3& position);
    void removeCamera(t_cameraPtr toRemove);

protected:


private:
    blub::async::dispatcher &m_worker;
    t_rendererSurface &m_terrain;

    t_syncRadiusList m_syncRadien;

};



}
}
}
}

#endif // VOXEL_TERRAIN_RENDERER_HPP
