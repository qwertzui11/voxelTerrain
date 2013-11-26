#ifndef VOXEL_EDIT_BASE_HPP
#define VOXEL_EDIT_BASE_HPP


#include "blub/core/enableSharedFromThis.hpp"
#include "blub/core/globals.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


class base : public enableSharedFromThis<base>
{
public:
    typedef enableSharedFromThis<base> t_base;
    typedef sharedPointer<base> pointer;

    virtual ~base();

    virtual blub::axisAlignedBox getAxisAlignedBoundingBox(const real& voxelSize, const transform& trans) const = 0;

    void calculateVoxel(const transform &trans) const;
    virtual void calculateVoxel(tile::container* voxelContainer, const vector3int32& voxelContainerOffset, const real& voxelScale, const transform &trans) const;

    void setCut(const bool& cut);
    const bool &getCut(void) const;

    void setVoxelContainer(simple::container::base* toSet);
    simple::container::base*  getVoxelContainer() const;

protected:
    base();
    virtual bool calculateOneVoxel(const vector3& pos, const real& voxelSize, data* resultVoxel) const = 0;

protected:
    simple::container::base*  m_voxelContainer;

    bool m_cut;

};


}
}
}
}


#endif // VOXEL_EDIT_BASE_HPP
