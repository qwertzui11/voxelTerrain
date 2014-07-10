#ifndef BLUB_MATH_INTERSECTION_HPP
#define BLUB_MATH_INTERSECTION_HPP

#include "blub/core/globals.hpp"


namespace blub
{


class intersection
{
public:
    static bool intersect(const ray &ray_, const triangleVector3 &triangle_, vector3* position);
};


}


#endif // BLUB_MATH_INTERSECTION_HPP
