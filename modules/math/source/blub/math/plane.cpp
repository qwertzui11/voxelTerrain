#include "blub/math/plane.hpp"

#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgrePlane.h>
#endif


using namespace blub;


#ifndef BLUB_NO_OGRE3D
plane::plane(const Ogre::Plane &vec)
    : normal(vec.normal)
    , d(vec.d)
{

}

plane::operator Ogre::Plane() const
{
    return Ogre::Plane(normal, d);
}
#endif

plane::plane()
{
}

plane::plane(const plane &other)
    : normal(other.normal)
    , d(other.d)
{
}

plane::plane(vector3 rkPoint0, vector3 rkPoint1, vector3 rkPoint2)
{
    vector3 kEdge1 = rkPoint1 - rkPoint0;
    vector3 kEdge2 = rkPoint2 - rkPoint0;
    normal = kEdge1.crossProduct(kEdge2);
    normal.normalise();
    d = -normal.dotProduct(rkPoint0);
}

plane::plane(vector3 rkPoint, vector3 rkNormal)
{
    normal = rkNormal;
    d = -rkNormal.dotProduct(rkPoint);
}

real plane::getDistance(const vector3 &rkPoint) const
{
    return normal.dotProduct(rkPoint) + d;
}



