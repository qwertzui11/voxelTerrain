#include "blub/math/vector3.hpp"

#include "blub/math/math.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/vector3int.hpp"

#include <boost/functional/hash.hpp>
#ifndef BLUB_NO_BULLET
#   include <LinearMath/btVector3.h>
#endif
#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgreVector3.h>
#endif


using namespace blub;

const vector3 vector3::ZERO = vector3(0.);
const vector3 vector3::UNIT_X = vector3(1., 0., 0.);
const vector3 vector3::UNIT_Y = vector3(0., 1., 0.);
const vector3 vector3::UNIT_Z = vector3(0., 0., 1.);
const vector3 vector3::NEGATIVE_UNIT_X = vector3(-1., 0., 0.);
const vector3 vector3::NEGATIVE_UNIT_Y = vector3(0., -1., 0.);
const vector3 vector3::NEGATIVE_UNIT_Z = vector3(0., 0., -1.);
const vector3 vector3::UNIT_SCALE = vector3(1., 1., 1.);


vector3::vector3(const vector3int32 &cast)
    : x((real)cast.x)
    , y((real)cast.y)
    , z((real)cast.z)
{
}

#ifndef BLUB_NO_OGRE3D
    vector3::vector3(const Ogre::Vector3 &vec)
        : x(vec.x)
        , y(vec.y)
        , z(vec.z)
    {;}

    vector3::operator Ogre::Vector3() const
    {
        return Ogre::Vector3(x, y, z);
    }
#endif
#ifndef BLUB_NO_BULLET
    vector3::vector3(const btVector3 &vec)
        : x(vec.getX())
        , y(vec.getY())
        , z(vec.getZ())
    {;}

    vector3::operator btVector3() const
    {
        return btVector3(x, y, z);
    }
#endif

vector3 vector3::getFloor() const
{
    return vector3(math::floor(x), math::floor(y), math::floor(z));
}

vector3 vector3::getAbs() const
{
    return vector3(math::abs(x), math::abs(y), math::abs(z));
}

quaternion vector3::getRotationTo(const vector3 &dest, const vector3 &fallbackAxis) const
{
    // Based on Stan Melax's article in Game Programming Gems
    quaternion q;
    // Copy, since cannot modify local
    vector3 v0 = *this;
    vector3 v1 = dest;
    v0.normalise();
    v1.normalise();

    real d = v0.dotProduct(v1);
    // If dot == 1, vectors are the same
    if (d >= 1.0f)
    {
        return quaternion::IDENTITY;
    }
    if (d < (1e-6f - 1.0f))
    {
        if (fallbackAxis != vector3::ZERO)
        {
            // rotate 180 degrees about the fallback axis
            q.FromAngleAxis(real(math::pi), fallbackAxis);
        }
        else
        {
            // Generate an axis
            vector3 axis = vector3::UNIT_X.crossProduct(*this);
            if (axis.isZeroLength()) // pick another if colinear
                axis = vector3::UNIT_Y.crossProduct(*this);
            axis.normalise();
            q.FromAngleAxis(real(math::pi), axis);
        }
    }
    else
    {
        real s = math::sqrt( (1+d)*2 );
        real invs = 1 / s;

        vector3 c = v0.crossProduct(v1);

        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s * 0.5f;
        q.normalise();
    }
    return q;
}


std::ostream &blub::operator<< (std::ostream &ostr, const vector3 &toCast)
{
    return ostr << "(" << toCast.x << "," << toCast.y << "," << toCast.z << ")";
}

std::size_t blub::hash_value(const vector3 &value)
{
    std::size_t result(value.x);
    boost::hash_combine(result, value.y);
    boost::hash_combine(result, value.z);

    return result;
}
