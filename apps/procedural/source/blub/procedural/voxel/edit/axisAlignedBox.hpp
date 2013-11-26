#ifndef PROCEDURAL_VOXEL_EDIT_AXISALIGNEDBOX_HPP
#define PROCEDURAL_VOXEL_EDIT_AXISALIGNEDBOX_HPP


#include "blub/math/axisAlignedBox.hpp"
#include "blub/procedural/voxel/edit/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


class axisAlignedBox : public base
{
public:
    typedef base t_base;
    typedef sharedPointer<axisAlignedBox> pointer;

    static pointer create(const ::blub::axisAlignedBox& desc);
    virtual ~axisAlignedBox();

    void setAxisAlignedBox(const ::blub::axisAlignedBox& desc);
    const ::blub::axisAlignedBox& getAxisAlignedBox(void) const;

    virtual blub::axisAlignedBox getAxisAlignedBoundingBox(const real& voxelSize, const transform& trans) const;

protected:
    virtual bool calculateOneVoxel(const vector3& pos, const real& voxelSize, data* resultVoxel) const;

private:
    axisAlignedBox(const ::blub::axisAlignedBox& desc);

private:
    ::blub::axisAlignedBox m_aab;

};


}
}
}
}


#endif // PROCEDURAL_VOXEL_EDIT_AXISALIGNEDBOX_HPP
