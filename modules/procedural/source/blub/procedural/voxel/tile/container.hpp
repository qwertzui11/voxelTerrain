#ifndef PROCEDURAL_VOXEL_TILE_CONTAINER_HPP
#define PROCEDURAL_VOXEL_TILE_CONTAINER_HPP

#include "blub/core/array.hpp"
#include "blub/core/classVersion.hpp"
#include "blub/core/vector.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/math/vector3int.hpp"
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


/**
 * @brief The container class contains an array of voxel. The amount of voxel per tile is voxelLength^3.
 * The class counts how many voxel are max and how many are min. if all voxel are min or max the class simple::container::base doesnt save them.
 * Additionally it saves an axisAlignedBox which describes the bounds of the voxel that changed.
 */
template <class configType>
class container : public base<container<configType> >
{
public:
    typedef configType t_config;
    typedef base<container<t_config> > t_base;
    typedef typename t_config::t_data t_data;

#if defined(BOOST_NO_CXX11_CONSTEXPR)
    static const int32 voxelLength;
    static const int32 voxelCount;
#else
    static constexpr int32 voxelLength = t_config::voxelsPerTile;
    static constexpr int32 voxelCount = voxelLength*voxelLength*voxelLength;
#endif
 
    typedef vector<t_data> t_voxelArray;

    /**
     * @brief create creates an instance.
     * @return never nullptr.
     */
    static typename t_base::pointer create()
    {
        return new container();
    }
    /**
     * @brief ~container destructor.
     */
    virtual ~container()
    {
    }

    /**
     * @brief startEdit resets the changed-voxel-bound-aab
     */
    void startEdit()
    {
        BASSERT(!m_editing);
        m_changedVoxelBoundingBox.setInvalid();
        m_editing = true;
    }
    /**
     * @brief endEdit end edit
     */
    void endEdit()
    {
        BASSERT(m_editing);
        m_editing = false;
    }
    /**
     * @brief getEditing returns if editing is active.
     * @return
     */
    const bool& getEditing() const
    {
        return m_editing;
    }

    /**
     * @brief getEditedVoxelBoundingBox returns an axisAlignedBox which describes the bounds of the voxel that changed since the last call of startEdit().
     * @return
     */
    const axisAlignedBoxInt32& getEditedVoxelBoundingBox() const
    {
        return m_changedVoxelBoundingBox;
    }

    /**
     * @brief setVoxel sets an voxel to a local position.
     * Extends changed axisAlignedBox-bounds and counts if voxel is minimum or maximum.
     * @param pos
     * @param toSet
     * @see procedural::voxel::data
     */
    void setVoxel(const vector3int32& pos, const t_data& toSet)
    {
        if(setVoxel(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }
    // TODO remove me - do lambda / boost/std::function
    void setVoxelIfInterpolationHigher(const vector3int32& pos, const t_data& toSet)
    {
        if(setVoxelIfInterpolationHigher(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }
    // TODO remove me - do lambda / boost/std::function
    void setVoxelIfInterpolationHigherEqualZero(const vector3int32& pos, const t_data& toSet)
    {
        if(setVoxelIfInterpolationHigherEqualZero(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }
    // TODO remove me - do lambda / boost/std::function
    void setVoxelIfInterpolationLower(const vector3int32& pos, const t_data& toSet)
    {
        if(setVoxelIfInterpolationLower(calculateIndex(pos), toSet))
        {
            m_changedVoxelBoundingBox.extend(pos);
        }
    }

    /**
     * @brief setFull sets all voxel to max.
     * @see procedural::voxel::data::setMax()
     */
    void setFull()
    {
        BASSERT(m_editing);

        for (uint32 indVoxel = 0; indVoxel < m_voxels.size(); ++indVoxel)
        {
            m_voxels[indVoxel].setMax();
        }
        m_countVoxelInterpolationLargerZero = voxelCount;
        m_countVoxelMinimum = 0;
        m_countVoxelMaximum = voxelCount;
    }
    /**
     * @brief setFull sets all voxel to max.
     * @see procedural::voxel::data::setMin()
     */
    void setEmpty()
    {
        BASSERT(m_editing);

        for (uint32 indVoxel = 0; indVoxel < m_voxels.size(); ++indVoxel)
        {
            m_voxels[indVoxel].setMin();
        }
        m_countVoxelInterpolationLargerZero = 0;
        m_countVoxelMinimum = voxelCount;
        m_countVoxelMaximum = 0;
    }

    /**
     * @brief getVoxel returns a copy of a local voxel.
     * @param pos local voxel-position.
     * @return
     */
    t_data getVoxel(const vector3int32& pos) const
    {
        return getVoxel(calculateIndex(pos));
    }
    /**
     * @brief getVoxel returns a copy of a local voxel.
     * @param index local voxel-position.
     * @return
     * @see calculateIndex()
     */
    const t_data& getVoxel(const int32& index) const
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);
        return m_voxels[index];
    }
    /**
     * @brief getVoxelArray returns reference voxel-array
     * @return
     */
    const t_voxelArray& getVoxelArray(void) const
    {
        return m_voxels;
    }

    /**
     * @brief calculateIndex convertes a 3d voxel-pos to a 1d array-index. 0 <= pos.xyz < voxelLength
     * @param pos to convert.
     * @return
     */
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

    /**
     * @brief getCountVoxelLargerZero returns number of voxel not minimum.
     * @return
     * @see procedural::voxel::data
     */
    const int32& getCountVoxelLargerZero() const
    {
        BASSERT(m_countVoxelInterpolationLargerZero >= 0);
        BASSERT(m_countVoxelInterpolationLargerZero <= voxelCount);
        return m_countVoxelInterpolationLargerZero;
    }
    /**
     * @brief getCountVoxelMaximum returns number of voxel that are maximum.
     * @return
     * @see procedural::voxel::data
     */
    const int32& getCountVoxelMaximum() const
    {
        BASSERT(m_countVoxelMaximum >= 0);
        BASSERT(m_countVoxelMaximum <= voxelCount);
        return m_countVoxelMaximum;
    }
    /**
     * @brief getCountVoxelMinimum returns number of voxel that are minimum.
     * @return
     * @see procedural::voxel::data
     */
    const int32& getCountVoxelMinimum() const
    {
        BASSERT(m_countVoxelMinimum >= 0);
        BASSERT(m_countVoxelMinimum <= voxelCount);
        return m_countVoxelMinimum;
    }

    /**
     * @brief isEmpty returns true if all voxel are minimum.
     * @return
     * @see procedural::voxel::data
     */
    bool isEmpty() const
    {
        return m_countVoxelMinimum == container::voxelCount;
    }

    /**
     * @brief isFull returns true if all voxel are maximum.
     * @return
     * @see procedural::voxel::data
     */
    bool isFull() const
    {
        return m_countVoxelMaximum == container::voxelCount;
    }

    /**
     * @brief operator = copy operator
     * @param other
     */
    void operator = (const container& other)
    {
        m_countVoxelInterpolationLargerZero = other.getCountVoxelLargerZero();
        m_countVoxelMinimum = other.getCountVoxelMinimum();
        m_countVoxelMaximum = other.getCountVoxelMaximum();
        m_voxels = other.getVoxelArray();
    }

protected:
    /**
     * @brief container constructor
     */
    container()
        : m_voxels(voxelCount)
        , m_countVoxelInterpolationLargerZero(0)
        , m_countVoxelMinimum(voxelCount)
        , m_countVoxelMaximum(0)
        , m_editing(false)
    {
        ;
    }

    // TODO remove me - do lambda / boost/std::function
    bool setVoxelIfInterpolationHigher(const int32& index, const t_data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).getInterpolation());
        if (currentInterpolation >= toSet.getInterpolation())
        {
            return false;
        }
        return setVoxel(index, toSet);
    }

    // TODO remove me - do lambda / boost/std::function
    bool setVoxelIfInterpolationHigherEqualZero(const int32& index, const t_data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).getInterpolation());
        if (currentInterpolation >= 0)
        {
            return false;
        }
        return setVoxel(index, toSet);
    }

    // TODO remove me - do lambda / boost/std::function
    bool setVoxelIfInterpolationLower(const int32& index, const t_data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const int8& currentInterpolation(getVoxel(index).getInterpolation());
        if (currentInterpolation <= toSet.getInterpolation())
        {
            return false;
        }
        return setVoxel(index, toSet);
    }

    /**
     * @brief setVoxel sets voxel to array and counts if voxel is maximum or minimum
     * @param index
     * @param toSet
     * @return
     * @see calculateIndex()
     * @see procedural::voxel::data
     */
    bool setVoxel(const int32& index, const t_data& toSet)
    {
        BASSERT(m_editing);
        BASSERT(index >= 0);
        BASSERT(index < voxelCount);

        const t_data& currentVoxel(getVoxel(index));

        if (currentVoxel == toSet)
        {
            return false;
        }

        // count for full/empty voxel --> memory optimisation
        if (currentVoxel.getInterpolation() < 0 && toSet.getInterpolation() >= 0)
        {
            ++m_countVoxelInterpolationLargerZero;
        }
        else
        {
            if (currentVoxel.getInterpolation() >= 0 && toSet.getInterpolation() < 0)
            {
                --m_countVoxelInterpolationLargerZero;
            }
        }
        if (!currentVoxel.isMin() && toSet.isMin())
        {
            ++m_countVoxelMinimum;
        }
        else
        {
            if (currentVoxel.isMin() && !toSet.isMin())
            {
                --m_countVoxelMinimum;
            }
        }
        if (!currentVoxel.isMax() && toSet.isMax())
        {
            ++m_countVoxelMaximum;
        }
        else
        {
            if (currentVoxel.isMax() && !toSet.isMax())
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
        readWrite & nameValuePair::create("countVoxelLargerZero", m_countVoxelInterpolationLargerZero);
        readWrite & nameValuePair::create("editing", m_editing);
        readWrite & nameValuePair::create("changedVoxelBoundingBox", m_changedVoxelBoundingBox);
        readWrite & nameValuePair::create("voxels", m_voxels);
    }

private:
    t_voxelArray m_voxels;

    int32 m_countVoxelInterpolationLargerZero;
    int32 m_countVoxelMinimum;
    int32 m_countVoxelMaximum;

    bool m_editing;
    axisAlignedBoxInt32 m_changedVoxelBoundingBox;

};


#if defined(BOOST_NO_CXX11_CONSTEXPR)
template <class voxelType>
const int32 container<voxelType>::voxelLength = t_config::voxelsPerTile;
template <class voxelType>
const int32 container<voxelType>::voxelCount = voxelLength*voxelLength*voxelLength;
#else
template <class voxelType>
constexpr int32 container<voxelType>::voxelLength;
template <class voxelType>
constexpr int32 container<voxelType>::voxelCount;
#endif


}
}
}
}




#endif // PROCEDURAL_VOXEL_TILE_CONTAINER_HPP
