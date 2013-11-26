#include "blub/math/axisAlignedBox.hpp"

#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/vector3int32.hpp"

#include <limits>

#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgreAxisAlignedBox.h>
#endif


using namespace blub;

axisAlignedBox::axisAlignedBox(const axisAlignedBoxInt32 &toCast)
    : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
{
    setExtents(vector3(toCast.getMinimum()), vector3(toCast.getMaximum()));
}

#ifndef BLUB_NO_OGRE3D
axisAlignedBox::axisAlignedBox(const Ogre::AxisAlignedBox &vec)
    : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
{
    setExtents(vector3(vec.getMinimum()), vector3(vec.getMaximum()));
}

axisAlignedBox::operator Ogre::AxisAlignedBox() const
{
    return Ogre::AxisAlignedBox(mMinimum, mMaximum);
}
#endif

real axisAlignedBox::volume() const
{
    switch (mExtent)
    {
    case EXTENT_NULL:
        return 0.0f;

    case EXTENT_FINITE:
        {
            vector3 diff = mMaximum - mMinimum;
            return diff.x * diff.y * diff.z;
        }

    case EXTENT_INFINITE:
        return std::numeric_limits<real>::infinity();

    default: // shut up compiler
        BASSERT( false && "Never reached" );
        return 0.0f;
    }
}

vector3 axisAlignedBox::getSize(void) const
{
    switch (mExtent)
    {
    case EXTENT_NULL:
        return vector3::ZERO;

    case EXTENT_FINITE:
        return mMaximum - mMinimum;

    case EXTENT_INFINITE:
        return vector3(
            std::numeric_limits<real>::infinity(),
            std::numeric_limits<real>::infinity(),
            std::numeric_limits<real>::infinity());

    default: // shut up compiler
        BASSERT( false && "Never reached" );
        return vector3::ZERO;
    }
}

vector3 axisAlignedBox::getHalfSize(void) const
{
    switch (mExtent)
    {
    case EXTENT_NULL:
        return vector3::ZERO;

    case EXTENT_FINITE:
        return (mMaximum - mMinimum) * 0.5;

    case EXTENT_INFINITE:
        return vector3(
            std::numeric_limits<real>::infinity(),
            std::numeric_limits<real>::infinity(),
            std::numeric_limits<real>::infinity());

    default: // shut up compiler
        BASSERT( false && "Never reached" );
        return vector3::ZERO;
    }
}

real axisAlignedBox::distance(const vector3 &v) const
{
    if (this->contains(v))
        return 0;
    else
    {
        real maxDist = std::numeric_limits<real>::min();

        if (v.x < mMinimum.x)
            maxDist = std::max(maxDist, mMinimum.x - v.x);
        if (v.y < mMinimum.y)
            maxDist = std::max(maxDist, mMinimum.y - v.y);
        if (v.z < mMinimum.z)
            maxDist = std::max(maxDist, mMinimum.z - v.z);

        if (v.x > mMaximum.x)
            maxDist = std::max(maxDist, v.x - mMaximum.x);
        if (v.y > mMaximum.y)
            maxDist = std::max(maxDist, v.y - mMaximum.y);
        if (v.z > mMaximum.z)
            maxDist = std::max(maxDist, v.z - mMaximum.z);

        return maxDist;
    }
}

