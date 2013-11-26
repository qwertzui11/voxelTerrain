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

