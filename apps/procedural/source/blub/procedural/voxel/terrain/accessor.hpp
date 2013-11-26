#ifndef PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP

#include "blub/core/globals.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{


class accessor : public base<sharedPointer<tile::accessor> >
{
public:
    accessor(blub::async::dispatcher &worker,
             simple::container::base &voxels,
             const uint32& numLod);
    ~accessor();

    simple::container::base &getVoxelContainer() const;

private:
    simple::container::base &m_voxels;

};


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP
