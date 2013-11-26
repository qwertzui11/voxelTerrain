#ifndef QUADVECTOR3_HPP
#define QUADVECTOR3_HPP

#include "blub/math/triangleVector3.hpp"

namespace blub
{

class quadVector3
{
public:
    quadVector3()
    {
        ind[0] = vector3(0);
        ind[1] = vector3(0);
        ind[2] = vector3(0);
        ind[3] = vector3(0);
    }

    quadVector3(vector3 a, vector3 b, vector3 c, vector3 d)
    {
        ind[0] = a;
        ind[1] = b;
        ind[2] = c;
        ind[3] = d;
    }
    quadVector3(triangleVector3 a, triangleVector3 b);

    bool operator == (const quadVector3 & other) const
    {
        return other.ind[0] == ind[0]
                && other.ind[1] == ind[1]
                && other.ind[2] == ind[2]
                && other.ind[3] == ind[3];
    }

    vector3 midPoint()
    {
        return (ind[0] + ind[1] + ind[2] + ind[3]) / 4.0;
    }

    bool combine(const quadVector3 &other, quadVector3* result);

    vector3 ind[4];
};

}

#endif // QUADVECTOR3_HPP
