#include "container.hpp"

#include "blub/core/log.hpp"

#include <string>


using namespace blub;


constexpr int32 procedural::voxel::tile::container::voxelLength;
constexpr int32 procedural::voxel::tile::container::voxelCount;
constexpr int8 procedural::voxel::tile::container::voxelInterpolationMaximum;
constexpr int8 procedural::voxel::tile::container::voxelInterpolationMinimum;


procedural::voxel::tile::container::t_base::pointer procedural::voxel::tile::container::create()
{
    return new container();
}

procedural::voxel::tile::container::~container()
{
}

void procedural::voxel::tile::container::setFull()
{
    BASSERT(m_editing);

    m_voxels.fill(voxelInterpolationMaximum);
    m_countVoxelLargerZero = voxelCount;
    m_countVoxelMinimum = 0;
    m_countVoxelMaximum = voxelCount;
}

void procedural::voxel::tile::container::setEmpty()
{
    BASSERT(m_editing);

    m_voxels.fill(voxelInterpolationMinimum);
    m_countVoxelLargerZero = 0;
    m_countVoxelMinimum = voxelCount;
    m_countVoxelMaximum = 0;
}

void procedural::voxel::tile::container::operator =(const procedural::voxel::tile::container &other)
{
    m_countVoxelLargerZero = other.getCountVoxelLargerZero();
    m_countVoxelMinimum = other.getCountVoxelMinimum();
    m_countVoxelMaximum = other.getCountVoxelMaximum();
    m_voxels = other.getVoxelArray();
}
