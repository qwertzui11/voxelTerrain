#include "sphere.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/math.hpp"
#include "blub/math/transform.hpp"
#include "blub/procedural/voxel/data.hpp"


using namespace blub::procedural::voxel::edit;
using namespace blub::procedural::voxel;


sphere::sphere(const blub::sphere &desc)
    : m_sphere(desc)
{
}

sphere::pointer sphere::create(const blub::sphere &desc)
{
    return pointer(new sphere(desc));
}

void sphere::setSphere(const blub::sphere &desc)
{
    m_sphere = desc;
}

const blub::sphere &sphere::getSphere() const
{
    return m_sphere;
}

blub::axisAlignedBox sphere::getAxisAlignedBoundingBox(const blub::real &voxelSize, const blub::transform &trans) const
{
    const blub::axisAlignedBox aabb(m_sphere.getCenter() - blub::vector3(m_sphere.getRadius()),
                              m_sphere.getCenter() + blub::vector3(m_sphere.getRadius()));
    return blub::axisAlignedBox(aabb.getMinimum() / voxelSize + trans.position,
                                aabb.getMaximum() / voxelSize + trans.position);
}

bool sphere::calculateOneVoxel(const blub::vector3 &pos, const blub::real &voxelSize, data *resultVoxel) const
{
    const blub::real squaredDist(pos.squaredDistance(m_sphere.getCenter()));
    const blub::real radius(m_sphere.getRadius());
    if (squaredDist < (radius+voxelSize)*(radius+voxelSize))
    {
        if (squaredDist < (radius-voxelSize)*(radius-voxelSize))
        {
            resultVoxel->interpolation = 127;
        }
        else
        {
            resultVoxel->interpolation = (blub::int8)blub::math::min((real)((radius-blub::math::sqrt(squaredDist))*127.0), (real)127.0);
        }
        return true;
    }
    else
    {
        // resultVoxel->interpolation = -127;
        return false;
    }
}
