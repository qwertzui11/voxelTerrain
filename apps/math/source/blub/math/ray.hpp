#ifndef LINE_HPP
#define LINE_HPP

#include "blub/math/vector3.hpp"


namespace Ogre
{
    class Ray;
}


namespace blub
{

class ray
{
public:
#ifndef BLUB_NO_OGRE3D
    ray(const Ogre::Ray &vec);
    operator Ogre::Ray() const;
#endif

    ray():mOrigin(vector3::ZERO), mDirection(vector3::UNIT_Z) {}
    ray(const vector3& origin, const vector3& direction)
        :mOrigin(origin), mDirection(direction) {}

    /** Sets the origin of the ray. */
    void setOrigin(const vector3& origin) {mOrigin = origin;}
    /** Gets the origin of the ray. */
    const vector3& getOrigin(void) const {return mOrigin;}

    /** Sets the direction of the ray. */
    void setDirection(const vector3& dir) {mDirection = dir;}
    /** Gets the direction of the ray. */
    const vector3& getDirection(void) const {return mDirection;}

    /** Gets the position of a point t units along the ray. */
    vector3 getPoint(real t) const {
        return vector3(mOrigin + (mDirection * t));
    }

    /** Gets the position of a point t units along the ray. */
    vector3 operator*(real t) const {
        return getPoint(t);
    }

    bool intersects (const plane &pl, vector3 *point = nullptr, real *tOut = nullptr) const;

protected:
    vector3 mOrigin;
    vector3 mDirection;

};

}

#endif // LINE_HPP
