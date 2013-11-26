#ifndef PROCEDURAL_VOXEL_TILE_CONTAINER_HPP
#define PROCEDURAL_VOXEL_TILE_CONTAINER_HPP

#include "blub/core/array.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/core/classVersion.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/data.hpp"
#include "blub/procedural/voxel/tile/base.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


class container : public base<container>
{
public:
    typedef base<container> t_base;

    static constexpr int32 voxelLength = 20;
    static constexpr int32 voxelCount = voxelLength*voxelLength*voxelLength;
    static constexpr int8 voxelInterpolationMaximum = 127;
    static constexpr int8 voxelInterpolationMinimum = -127;

    typedef array<data, voxelCount> t_voxelArray;

    static pointer create();
    virtual ~container();

    void startEdit(void)
    {
        BASSERT(!m_editing);
        m_changedVoxelBoundingBox.setInvalid();
        m_editing = true;
    }
    void endEdit(void)
    {
        BASSERT(m_editing);
        m_editing = false;
    }
    const bool& getEditing() const
    {
        return m_editing;
    }

    const axisAlignedBoxInt32& getEditedVoxelBoundingBox(void) const
    {
        return m_changedVoxelBoundingBox;
    }

    void setVoxel(const vector3int32& pos, const data& toSet)
    {
        if(setVoxel(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }
    void setVoxelIfInterpolationHigher(const vector3int32& pos, const data& toSet)
    {
        if(setVoxelIfInterpolationHigher(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }
    void setVoxelIfInterpolationLower(const vector3int32& pos, const data& toSet)
    {
        if(setVoxelIfInterpolationLower(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }

    void setFull(void);
    void setEmpty(void);

    data getVoxel(const vector3int32& pos) const
    {
        return getVoxel(calculateIndex(pos));
    }
    const data& getVoxel(const int32& index) const
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);
        return m_voxels[index];
    }
    const t_voxelArray& getVoxelArray(void) const
    {
        return m_voxels;
    }

    static int32 calculateIndex(const vector3int32& pos)
    {
        BASSERT(pos.x >= 0);
        BASSERT(pos.y >= 0);
        BASSERT(pos.z >= 0);
        BASSERT(pos.x < voxelLength);
        BASSERT(pos.y < voxelLength);
        BASSERT(pos.z < voxelLength);
        return (pos.x)*(voxelLength*voxelLength) + (pos.y)*(voxelLength) + (pos.z);
    }

    const int32& getCountVoxelLargerZero(void) const
    {
        BASSERT(m_countVoxelLargerZero >= 0);
        BASSERT(m_countVoxelLargerZero <= voxelCount);
        return m_countVoxelLargerZero;
    }
    const int32& getCountVoxelMaximum(void) const
    {
        BASSERT(m_countVoxelMaximum >= 0);
        BASSERT(m_countVoxelMaximum <= voxelCount);
        return m_countVoxelMaximum;
    }
    const int32& getCountVoxelMinimum(void) const
    {
        BASSERT(m_countVoxelMinimum >= 0);
        BASSERT(m_countVoxelMinimum <= voxelCount);
        return m_countVoxelMinimum;
    }

    bool isEmpty(void) const
    {
        return m_countVoxelMinimum == container::voxelCount;
    }

    bool isFull(void) const
    {
        return m_countVoxelMaximum == container::voxelCount;
    }

    void operator = (const container& other);

protected:
    container()
        : m_countVoxelLargerZero(0)
        , m_countVoxelMinimum(voxelCount)
        , m_countVoxelMaximum(0)
        , m_editing(false)
    {
        ;
    }

    bool setVoxelIfInterpolationHigher(const int32& index, const data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).interpolation);
        if (currentInterpolation >= toSet.interpolation)
        {
            return false;
        }
        return setVoxel(index, toSet);
    }
    bool setVoxelIfInterpolationLower(const int32& index, const data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).interpolation);
        if (currentInterpolation <= toSet.interpolation)
        {
            return false;
        }
        return setVoxel(index, toSet);
    }

    bool setVoxel(const int32& index, const data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).interpolation);

        if (currentInterpolation == toSet.interpolation)
        {
            return false;
        }

        // count for full/empty voxel --> memory optimisation
        if (currentInterpolation <= 0 && toSet.interpolation > 0)
        {
            ++m_countVoxelLargerZero;
        }
        else
        {
            if (currentInterpolation > 0 && toSet.interpolation <= 0)
            {
                --m_countVoxelLargerZero;
            }
        }
        if (currentInterpolation != voxelInterpolationMinimum && toSet.interpolation == voxelInterpolationMinimum)
        {
            ++m_countVoxelMinimum;
        }
        else
        {
            if (currentInterpolation == voxelInterpolationMinimum && toSet.interpolation != voxelInterpolationMinimum)
            {
                --m_countVoxelMinimum;
            }
        }
        if (currentInterpolation != voxelInterpolationMaximum && toSet.interpolation == voxelInterpolationMaximum)
        {
            ++m_countVoxelMaximum;
        }
        else
        {
            if (currentInterpolation == voxelInterpolationMaximum && toSet.interpolation != voxelInterpolationMaximum)
            {
                --m_countVoxelMaximum;
            }
        }

        // actually set the voxel
        m_voxels[index] = toSet;

        return true;
    }

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        (void)version;

        readWrite & nameValuePair::create("countVoxelMinimum", m_countVoxelMinimum);
        readWrite & nameValuePair::create("countVoxelMaximum", m_countVoxelMaximum);
        readWrite & nameValuePair::create("countVoxelLargerZero", m_countVoxelLargerZero);
        readWrite & nameValuePair::create("editing", m_editing);
        readWrite & nameValuePair::create("changedVoxelBoundingBox", m_changedVoxelBoundingBox);
        readWrite & nameValuePair::create("voxels", m_voxels);
    }

private:
    t_voxelArray m_voxels;

    int32 m_countVoxelLargerZero;
    int32 m_countVoxelMinimum;
    int32 m_countVoxelMaximum;

    bool m_editing;
    axisAlignedBoxInt32 m_changedVoxelBoundingBox;

};


}
}
}
}
BLUB_CLASSVERSION(blub::procedural::voxel::tile::container, 1)



#endif // PROCEDURAL_VOXEL_TILE_CONTAINER_HPP
