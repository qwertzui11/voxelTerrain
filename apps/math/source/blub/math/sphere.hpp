#ifndef BLUB_SPHERE_HPP
#define BLUB_SPHERE_HPP

#include "vector3.hpp"


namespace blub
{

    class sphere
    {
    public:
        /** Standard constructor - creates a unit sphere around the origin.*/
        sphere() : mRadius(1.0), mCenter(vector3::ZERO) {}
        /** Constructor allowing arbitrary spheres.
            @param center The center point of the sphere.
            @param radius The radius of the sphere.
        */
        sphere(const vector3& center, real radius)
            : mRadius(radius), mCenter(center) {}

        /** Returns the radius of the sphere. */
        real getRadius(void) const { return mRadius; }

        /** Sets the radius of the sphere. */
        void setRadius(real radius) { mRadius = radius; }

        /** Returns the center point of the sphere. */
        const vector3& getCenter(void) const { return mCenter; }

        /** Sets the center point of the sphere. */
        void setCenter(const vector3& center) { mCenter = center; }

        /** Returns whether or not this sphere intersects another sphere. */
        bool intersects(const sphere& s) const;

        /** Returns whether or not this sphere intersects a box. */
        bool intersects(const axisAlignedBox& box) const;

        /** Returns whether or not this sphere intersects a point. */
        bool intersects(const vector3& v) const
        {
            return ((v - mCenter).squaredLength() <= mRadius*mRadius);
        }
        /** Merges another sphere into the current sphere */
        void merge(const sphere& oth);

        bool contains(const vector3& vec) const
        {return intersects(vec);}

        real getSquaredRadius() const
        {
            return mRadius*mRadius;
        }

        bool inherts(axisAlignedBox aabb) const;

    protected:
        real mRadius;
        vector3 mCenter;

    };

}

#endif // BLUB_SPHERE_HPP
