#ifndef PROCEDURAL_VOXEL_TERRAIN_SURFACE_HPP
#define PROCEDURAL_VOXEL_TERRAIN_SURFACE_HPP

#include "blub/core/list.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{


class surface : public base<sharedPointer<tile::surface> >
{
public:
    typedef simple::surface* t_lodPtr;
    typedef base<sharedPointer<tile::surface> > t_base;

    typedef base<sharedPointer<tile::accessor> > t_terrainAccessor;

    surface(blub::async::dispatcher &worker, t_terrainAccessor &voxels, real voxelSize);
    virtual ~surface();

private:


};


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_SURFACE_HPP
