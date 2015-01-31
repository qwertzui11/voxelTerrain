#include "ray.hpp"

#include "blub/math/plane.hpp"
#include "blub/log/global.hpp"

#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgreRay.h>
#endif


using namespace blub;


#ifndef BLUB_NO_OGRE3D
ray::ray(const Ogre::Ray &vec)
    : mOrigin(vec.getOrigin())
    , mDirection(vec.getDirection())
{
    ;
}

ray::operator Ogre::Ray() const
{
    return Ogre::Ray(mOrigin, mDirection);
}
#endif


bool ray::intersects(const plane &pl, vector3 *point, real *tOut) const
{
    std::pair<bool, real> result;
    real denom = pl.normal.dotProduct(getDirection());
    if (math::abs(denom) < std::numeric_limits<real>::epsilon())
    {
        // Parallel
        result = std::pair<bool, real>(false, 0);
    }
    else
    {
        real nom = pl.normal.dotProduct(getOrigin()) + pl.d;
        real t = -(nom/denom);
        result = std::pair<bool, real>(true, t);
    }

    if (result.first)
    {
        if (point)
        {
            *point = getPoint(result.second);
        }
        if (tOut)
        {
            *tOut = result.second;
        }
    }
    return result.first;
}

