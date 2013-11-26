#ifndef VOXEL_EDIT_SPHERE_HPP
#define VOXEL_EDIT_SPHERE_HPP


#include "blub/math/sphere.hpp"
#include "blub/procedural/voxel/edit/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


class sphere : public base
{
public:
    typedef base t_base;
    typedef sharedPointer<sphere> pointer;

    static pointer create(const ::blub::sphere& desc);

    void setSphere(const ::blub::sphere& desc);
    const ::blub::sphere& getSphere(void) const;

    virtual blub::axisAlignedBox getAxisAlignedBoundingBox(const real& voxelSize, const blub::transform &trans) const;

protected:
    virtual bool calculateOneVoxel(const vector3& pos, const real& voxelSize, data* resultVoxel) const;

private:
    sphere(const ::blub::sphere& desc);

private:
    ::blub::sphere m_sphere;

};


}
}
}
}


#endif // VOXEL_EDIT_SPHERE_HPP
