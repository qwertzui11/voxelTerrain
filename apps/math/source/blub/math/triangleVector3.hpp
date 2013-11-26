#ifndef TRIANGLEVECTOR3_2_HPP
#define TRIANGLEVECTOR3_2_HPP

#include "blub/math/vector3.hpp"


namespace blub
{

class triangleVector3
{
public:
    triangleVector3()
    {
        ind[0] = vector3(0);
        ind[1] = vector3(0);
        ind[2] = vector3(0);
    }
    triangleVector3(const vector3& a, const vector3& b, const vector3& c)
    {
        ind[0] = a;
        ind[1] = b;
        ind[2] = c;
    }

    triangleVector3(const vector3& a)
    {
        ind[0] = a;
        ind[1] = a;
        ind[2] = a;
    }

    bool operator == (const triangleVector3 & other) const
    {
        return other.ind[0] == ind[0]
                && other.ind[1] == ind[1]
                && other.ind[2] == ind[2];
    }

    vector3 nearest(vector3 to) const
    {
        return ind[nearestId(to)];
    }

    uint16 nearestId(vector3 to) const
    {
        uint16 nearestId(0);
        real nearest(ind[0].squaredDistance(to));
        for (uint16 index = 1; index < 3; ++index)
        {
            if (ind[index].squaredDistance(to) < nearest)
                nearestId = index;
        }
        return nearestId;
    }

    vector3 normal() const
    {
        return (ind[1]-ind[0]).crossProduct(ind[2]-ind[0]).normalisedCopy();
    }

/*
    plane getPlane()
    {
        return plane(ind[0], ind[1], ind[2]);
    }

    vector3 midPoint()
    {
        return (ind[0] + ind[1] + ind[2]) / 3.0;

        line3 ln1(ind[0], ind[1] - ind[0]);
        line3 ln2(ind[1], ind[2] - ind[1]);
        line3 ln3(ln1.getPoint(0.5), ind[2] - ln1.getPoint(0.5));
        line3 ln4(ln2.getPoint(0.5), ind[0] - ln2.getPoint(0.5));
        vector3 mid;
        if (ln3.intersects(ln4, &mid))
        {
            return mid;
        }
        else
            BERROR("FATAL ln3.intersects(ln4, &mid");
        return ind[0];
    }
*/
    vector3 ind[3];
};

}

#endif // TRIANGLEVECTOR3_HPP
