#ifndef BLUB_PROCEDURAL_VOXEL_CONFIG
#define BLUB_PROCEDURAL_VOXEL_CONFIG

#include "blub/procedural/predecl.hpp"
#include "blub/procedural/voxel/data.hpp"
#include "blub/procedural/voxel/vertex.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{


class config
{
public:
    typedef config t_config;

    typedef data t_data;
    typedef uint16 t_index;
    typedef vertex t_vertex;

    static const int32 voxelsPerTile = 20; // means 20^3!

    template <typename configType>
    struct container
    {
        typedef voxel::simple::container::inMemory<configType> t_simple;
        typedef voxel::tile::container<configType> t_tile;
    };
    typedef container<config> t_container;

    template <typename configType>
    struct accessor
    {
        typedef voxel::simple::accessor<configType> t_simple;
        typedef voxel::terrain::accessor<configType> t_terrain;
        typedef voxel::tile::accessor<configType> t_tile;
    };

    typedef accessor<config> t_accessor;

    template <typename configType>
    struct surface
    {
        typedef voxel::simple::surface<configType> t_simple;
        typedef voxel::terrain::surface<configType> t_terrain;
        typedef voxel::tile::surface<configType> t_tile;
    };
    typedef surface<config> t_surface;

    template <typename configType>
    struct renderer
    {
        typedef voxel::simple::renderer<configType> t_simple;
        typedef voxel::terrain::renderer<configType> t_terrain;
        typedef voxel::tile::renderer<configType> t_tile;
    };
    typedef renderer<config> t_renderer;
};


}
}
}


#endif // BLUB_PROCEDURAL_VOXEL_CONFIG

