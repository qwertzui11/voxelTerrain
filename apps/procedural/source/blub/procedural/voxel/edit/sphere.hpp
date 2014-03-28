#ifndef BLUB_PROCEDURAL_VOXEL_EDIT_SPHERE_HPP
#define BLUB_PROCEDURAL_VOXEL_EDIT_SPHERE_HPP


// #include "blub/core/log.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/math.hpp"
#include "blub/math/sphere.hpp"
#include "blub/math/transform.hpp"
#include "blub/procedural/voxel/edit/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


/**
 * @brief The sphere class creates an interpolated voxel-sphere
 */
template <class voxelType>
class sphere : public base<voxelType>
{
public:
    typedef base<voxelType> t_base;
    typedef sharedPointer<sphere<voxelType> > pointer;

    /**
     * @brief creates an instance
     * @param desc Describes the voxels to create
     * @return an instance of sphere as shared_ptr
     */
    static pointer create(const ::blub::sphere& desc)
    {
        return pointer(new sphere(desc));
    }

    /**
     * @brief setSphere sets the describing sphere. Dont set it while caluculating voxel.
     * @param desc
     */
    void setSphere(const ::blub::sphere& desc)
    {
        m_sphere = desc;
    }
    /**
     * @brief getSphere
     * @return Returns the describing sphere
     */
    const ::blub::sphere& getSphere() const
    {
        return m_sphere;
    }

    /**
     * @brief getAxisAlignedBoundingBox returns the transformed aab that includes the sphere.
     * @param trans Transform.
     * @return
     */
    blub::axisAlignedBox getAxisAlignedBoundingBox(const blub::transform &trans) const override
    {
        const blub::axisAlignedBox aabb(m_sphere.getCenter() - blub::vector3(m_sphere.getRadius()),
                                  m_sphere.getCenter() + blub::vector3(m_sphere.getRadius()));
        return blub::axisAlignedBox(aabb.getMinimum()*trans.scale + trans.position,
                                    aabb.getMaximum()*trans.scale + trans.position);
    }

protected:
    /**
     * @brief calculateOneVoxel calculates on voxel in getAxisAlignedBoundingBox().
     * @param pos absolute voxel position.
     * @param resultVoxel interpolation gets set if inside sphere.
     * @return true if voxel is inside sphere-radius+1. +1 because of interpolation.
     */
    bool calculateOneVoxel(const vector3& pos, voxelType* resultVoxel) const override
    {
        const blub::real &squaredDist(pos.squaredDistance(m_sphere.getCenter()));
        const blub::real &radius(m_sphere.getRadius());
        if (squaredDist < (radius+1.)*(radius+1.))
        {
            if (squaredDist < (radius-1.)*(radius-1.))
            {
                resultVoxel->setInterpolationMax();
            }
            else
            {
                const blub::real &result(blub::math::clamp<blub::real>((radius-blub::math::sqrt(squaredDist))*127., -127., 127.));
                resultVoxel->setInterpolation(static_cast<blub::int8>(result));
            }
            return true;
        }
        return false;
    }

private:
    sphere(const ::blub::sphere& desc)
        : m_sphere(desc)
    {
    }

private:
    ::blub::sphere m_sphere;

};


}
}
}
}


#endif // VOXEL_EDIT_SPHERE_HPP
