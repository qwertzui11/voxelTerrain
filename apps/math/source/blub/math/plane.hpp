#ifndef PLANE_HPP
#define PLANE_HPP

#include "blub/math/vector3.hpp"


namespace Ogre
{
    class Plane;
}


namespace blub
{

class plane
{
public:
#ifndef BLUB_NO_OGRE3D
    plane(const Ogre::Plane &vec);
    operator Ogre::Plane() const;
#endif

    plane();
    plane(const plane& other);
    plane(vector3 p0, vector3 p1, vector3 p2);
    plane(vector3 pos, vector3 normal);

    /** This is a pseudodistance. The sign of the return value is
        positive if the point is on the positive side of the plane,
        negative if the point is on the negative side, and zero if the
        point is on the plane.
        @par
        The absolute value of the return value is the true distance only
        when the plane normal is a unit length vector.
    */
    real getDistance (const vector3 &rkPoint) const;

    bool operator==(const plane& rhs) const
    {
        return rhs.d == d && rhs.normal == normal;
    }
    bool operator!=(const plane& rhs) const
    {
        return rhs.d != d || rhs.normal != normal;
    }

public:
    vector3 normal;
    real d;

};

}

#endif // PLANE_HPP

