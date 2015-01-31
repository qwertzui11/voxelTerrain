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


/**
 * @brief The surface class contains a custom count of simple::surface for level of detail.
 */
template <class configType>
class surface : public base<typename configType::t_surface::t_simple>
{
public:
    typedef configType t_config;
    typedef typename t_config::t_surface::t_simple t_simple;
    typedef t_simple t_lod;
    typedef t_lod* t_lodPtr;
    typedef base<t_simple> t_base;

    typedef typename t_config::t_accessor::t_terrain t_terrainAccessor;


    /**
     * @brief surface construtor. Creates as much lods as in voxels.
     * @param worker may get called by multiple threads.
     * @param voxels The accessor to sync with.
     */
    surface(blub::async::dispatcher &worker, t_terrainAccessor &voxels)
    {
        for (int32 lod = 0; lod < voxels.getNumLod(); ++lod)
        {
            typename t_terrainAccessor::t_lod accessorTiles(voxels.getLod(lod));
            t_lodPtr newLod(new t_lod(worker, *accessorTiles, lod));

            t_base::m_lods.emplace_back(newLod);
        }
    }

    /**
     * @brief ~surface destrcutor
     */
    ~surface()
    {
    }

private:


};


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_SURFACE_HPP
