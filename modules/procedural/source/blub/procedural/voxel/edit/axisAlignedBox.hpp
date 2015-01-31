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


/**
 * @brief insertes/removes voxels that are included in an blub::axisAlignedBox.
 * Always axis-aligned - use box if you want to rotate.
 */
template <class configType>
class axisAlignedBox : public base<configType>
{
public:
    typedef configType t_config;
    typedef base<configType> t_base;
    typedef sharedPointer<axisAlignedBox> pointer;
    typedef typename t_config::t_data t_voxel;

    /**
     * @brief Allocates an instance and initialises the class
     * @param desc The axis-aligend-box that describes the to create voxel. desc must be a valid blub::axisAlignedBox.
     * @return Returns an initialised instance
     */
    static pointer create(const ::blub::axisAlignedBox& desc)
    {
        return pointer(new axisAlignedBox(desc));
    }
    /**
     * @brief Destructor
     */
    virtual ~axisAlignedBox()
    {
        ;
    }

    /**
     * @brief Changes the voxel-describing bounding-box. DON'T change it while class is generating voxel.
     * Else incorrect and incosinstent voxel will get created.
     * @param desc The Voxel descriptor.
     */
    void setAxisAlignedBox(const ::blub::axisAlignedBox& desc)
    {
        m_aab = desc;
    }
    /**
     * @brief Returns the describing blub::axisAlignedBox set by constructor or by setAxisAlignedBox().
     * @return Returns the describing blub::axisAlignedBox
     */
    const ::blub::axisAlignedBox& getAxisAlignedBox() const
    {
        return m_aab;
    }

protected:
    /**
     * @brief returns the bounding box describing the voxel to be recalculated
     * @param trans describes the tranformation the edit uses
     */
    blub::axisAlignedBox getAxisAlignedBoundingBox(const transform& trans) const override
    {
        return blub::axisAlignedBox(m_aab.getMinimum()*trans.scale + trans.position,
                                    m_aab.getMaximum()*trans.scale + trans.position);
    }

    /**
     * @brief checks if voxel is inside aab
     * @param pos describes the voxel-position
     * @param resultVoxel if pos is inside, resultVoxel interpolation gets set to maximum. Must not be nullptr
     * @return true if pos is inside.
     */
    bool calculateOneVoxel(const vector3& pos, t_voxel* resultVoxel) const override
    {
        if (m_aab.contains(pos))
        {
            resultVoxel->setInterpolationMax();
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    /**
     * @brief see create()
     */
    axisAlignedBox(const ::blub::axisAlignedBox& desc)
        : m_aab(desc)
    {
        ;
    }

private:
    ::blub::axisAlignedBox m_aab;


};


}
}
}
}


#endif // PROCEDURAL_VOXEL_EDIT_AXISALIGNEDBOX_HPP
