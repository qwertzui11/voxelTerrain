#include "surface.hpp"

#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/simple/surface.hpp"


using namespace blub::procedural::voxel::terrain;
using namespace blub::procedural::voxel;
using namespace blub;


surface::surface(async::dispatcher &worker, t_terrainAccessor &voxels, real voxelSize)
{
    for (uint32 lod = 0; lod < voxels.getNumLod(); ++lod)
    {
        t_terrainAccessor::t_lod accessorTiles(voxels.getLod(lod));
        t_lodPtr newLod(new simple::surface(worker, *accessorTiles, lod, voxelSize));
        voxelSize*=2.;

        m_lods.push_back(newLod);
    }
}


surface::~surface()
{
    for (auto lod : m_lods)
    {
        delete lod;
    }
    m_lods.clear();
}

