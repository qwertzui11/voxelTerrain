#include "accessor.hpp"

#include "blub/core/sharedPointer.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"


using namespace blub::procedural::voxel::terrain;
using namespace blub;


accessor::accessor(blub::async::dispatcher &worker, simple::container::base &voxels,
                   const uint32& numLod)
    : m_voxels(voxels)
{
    for (uint32 indLod = 0; indLod < numLod; ++indLod)
    {
        simple::accessor* lod = new simple::accessor(worker, voxels, indLod);
        m_lods.push_back(lod);
    }
}

accessor::~accessor()
{
    for (auto lod : m_lods)
    {
        delete lod;
    }
}

procedural::voxel::simple::container::base &accessor::getVoxelContainer() const
{
    return m_voxels;
}
