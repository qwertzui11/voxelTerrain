#include "intersection.hpp"

#include "blub/math/math.hpp"
#include "blub/math/ray.hpp"
#include "blub/math/triangleVector3.hpp"
#include "blub/math/vector3.hpp"

#include <limits>


bool blub::intersection::intersect(const blub::ray &ray_, const blub::triangleVector3 &triangle_, blub::vector3 *position)
{
    //
    // Calculate intersection with plane.
    //
    const bool negativeSide(true);
    const bool positiveSide(true);
    const vector3 normal(triangle_.getNormal());
    const vector3 posA(triangle_.positions[0]);
    const vector3 posB(triangle_.positions[1]);
    const vector3 posC(triangle_.positions[2]);

    real t;
    {
        real denom = normal.dotProduct(ray_.getDirection());

        // Check intersect side
        if (denom > + std::numeric_limits<real>::epsilon())
        {
            if (!negativeSide)
            {
                return false;
            }
        }
        else if (denom < - std::numeric_limits<real>::epsilon())
        {
            if (!positiveSide)
            {
                return false;
            }
        }
        else
        {
            // Parallel or triangle area is close to zero when
            // the plane normal not normalised.
            return false;
        }

        t = normal.dotProduct(posA - ray_.getOrigin()) / denom;
    }

    //
    // Calculate the largest area projection plane in X, Y or Z.
    //
    size_t i0, i1;
    {
        real n0 = math::abs(normal[0]);
        real n1 = math::abs(normal[1]);
        real n2 = math::abs(normal[2]);

        i0 = 1; i1 = 2;
        if (n1 > n2)
        {
            if (n1 > n0) i0 = 0;
        }
        else
        {
            if (n2 > n0) i1 = 0;
        }
    }

    //
    // Check the intersection point is inside the triangle.
    //
    {
        real u1 = posB[i0] - posA[i0];
        real v1 = posB[i1] - posA[i1];
        real u2 = posC[i0] - posA[i0];
        real v2 = posC[i1] - posA[i1];
        real u0 = t * ray_.getDirection()[i0] + ray_.getOrigin()[i0] - posA[i0];
        real v0 = t * ray_.getDirection()[i1] + ray_.getOrigin()[i1] - posA[i1];

        real alpha = u0 * v2 - u2 * v0;
        real beta  = u1 * v0 - u0 * v1;
        real area  = u1 * v2 - u2 * v1;

        // epsilon to avoid float precision error
        const real EPSILON = 1e-6f; // changed from ogre3d 1e-6f

        real tolerance = - EPSILON * area;

        if (area > 0)
        {
            if (alpha < tolerance || beta < tolerance || alpha+beta > area-tolerance)
            {
                return false;
            }
        }
        else
        {
            if (alpha > tolerance || beta > tolerance || alpha+beta < area-tolerance)
            {
                return false;
            }
        }
    }

    if (position != nullptr)
    {
        *position = ray_.getPoint(t);
    }

    return true;
}
