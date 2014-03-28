#ifndef VOXEL_DATA_HPP
#define VOXEL_DATA_HPP

#include "blub/core/classVersion.hpp"
#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{


/**
 * @brief The data class is the default voxel.
 * Contains an 8-bit interpolation value.
 * Replace/derive it and set it tho the voxel classes by template.
 * Watchout when you reimplement a isMax() and isMin().
 * voxel-container doesnt save these voxel for memory-optimisation.
 * Only return true on isMax() / isMin() if your are able to restore the state by setMin() / setMax().
 * setMin() has to to have the same results like the default contructor.
 */
class data
{
public:
    /**
     * @brief data constructor
     */
    data()
        : interpolation(-127)
    {
        ;
    }
    /**
     * @brief operator == compares interpolation.
     * @param other
     * @return
     */
    bool operator ==(const data& other) const
    {
        return other.interpolation == interpolation;
    }
    /**
     * @brief operator != compares interpolation.
     * @param other
     * @return
     */
    bool operator !=(const data& other) const
    {
        return other.interpolation != interpolation;
    }

    /**
     * @brief setInterpolation sets interpolation
     * @param toSet
     */
    void setInterpolation(const int8 &toSet)
    {
        interpolation = toSet;
    }

    /**
     * @brief getInterpolation returns reference to interpolation.
     * @return
     */
    int8 &getInterpolation()
    {
        return interpolation;
    }

    /**
     * @brief getInterpolation returns const reference to interpolation.
     * @return
     */
    const int8 &getInterpolation() const
    {
        return interpolation;
    }

    /**
     * @brief isMax checks if all values are maximum. See class description.
     * @return
     */
    bool isMax() const
    {
        return interpolation >= 127;
    }

    /**
     * @brief isMin checks if all values are minimum. See class description.
     * @return
     */
    bool isMin() const
    {
        return interpolation <= -127;
    }

    /**
     * @brief setMin sets all values to minimum. See class description.
     */
    void setMin()
    {
        setInterpolationMin();
    }

    /**
     * @brief setMax sets all values to maximum. See class description.
     */
    void setMax()
    {
        setInterpolationMax();
    }

    /**
     * @brief setInterpolationMin sets the interpolation to minimum.
     */
    void setInterpolationMin()
    {
        interpolation = -127;
    }

    /**
     * @brief setInterpolationMax set interpolation to maximum.
     */
    void setInterpolationMax()
    {
        interpolation = 127;
    }

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(interpolation);
    }


protected:
    /**
     * @brief interpolation signed 8-bit interpolation value default used by voxel::tile::surface
     */
    int8 interpolation;


};


}
}
}


#endif // VOXEL_DATA_HPP
