#include "accessor.hpp"

#include "blub/core/log.hpp"

#include <string>


using namespace blub;


constexpr int32 procedural::voxel::tile::accessor::voxelLength;
constexpr int32 procedural::voxel::tile::accessor::voxelLengthWithNormalCorrection;
constexpr int32 procedural::voxel::tile::accessor::voxelLengthLod;
constexpr int32 procedural::voxel::tile::accessor::voxelCount;
constexpr int32 procedural::voxel::tile::accessor::voxelCountLod;
constexpr int32 procedural::voxel::tile::accessor::voxelCountLodAll;
constexpr int32 procedural::voxel::tile::accessor::voxelLengthSurface;
constexpr int32 procedural::voxel::tile::accessor::voxelCountSurface;


procedural::voxel::tile::accessor::t_base::pointer procedural::voxel::tile::accessor::create(const bool &calculateLod)
{
    return new accessor(calculateLod);
}

procedural::voxel::tile::accessor::~accessor()
{
    // blub::BOUT("tile::~tile()");

    if (m_calculateLod)
    {
        delete m_voxelsLod;
        m_voxelsLod = nullptr;
    }
}

void procedural::voxel::tile::accessor::setCalculateLod(const bool &lod)
{
    if (m_calculateLod == lod)
    {
        // do nothing
        BWARNING("m_calculateLod == lod");
        return;
    }
    m_calculateLod = lod;
    if (m_calculateLod)
    {
        BASSERT(m_voxelsLod == nullptr);
        m_voxelsLod = new t_voxelArrayLod(); // [6*voxelCountLod]
    }
    else
    {
        delete m_voxelsLod;
        m_voxelsLod = nullptr;
    }
}

void procedural::voxel::tile::accessor::setValueToAllVoxel(const int8 &value)
{
    if (value < 0)
    {
        m_numVoxelLargerZeroLod = m_numVoxelLargerZero = 0;
    }
    else
    {
        if (m_calculateLod)
        {
            m_numVoxelLargerZeroLod = voxelCountLodAll;
            m_numVoxelLargerZero = voxelCountSurface;
        }
        else
        {
            m_numVoxelLargerZero = voxelCountSurface;
        }
    }

    m_voxels.fill(value);
    if (m_calculateLod)
    {
        memset(m_voxelsLod, (unsigned)value, 6*voxelCountLod);
    }
}
