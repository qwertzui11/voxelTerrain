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
        positions[0] = vector3(0);
        positions[1] = vector3(0);
        positions[2] = vector3(0);
    }
    triangleVector3(const vector3& a, const vector3& b, const vector3& c)
    {
        positions[0] = a;
        positions[1] = b;
        positions[2] = c;
    }

    triangleVector3(const vector3& a)
    {
        positions[0] = a;
        positions[1] = a;
        positions[2] = a;
    }

    bool operator == (const triangleVector3 & other) const
    {
        return other.positions[0] == positions[0]
                && other.positions[1] == positions[1]
                && other.positions[2] == positions[2];
    }

    vector3 nearest(vector3 to) const
    {
        return positions[nearestId(to)];
    }

    uint16 nearestId(vector3 to) const
    {
        uint16 nearestId(0);
        real nearest(positions[0].squaredDistance(to));
        for (uint16 index = 1; index < 3; ++index)
        {
            if (positions[index].squaredDistance(to) < nearest)
                nearestId = index;
        }
        return nearestId;
    }

    vector3 getNormal() const
    {
        return ((positions[1]-positions[0]).normalisedCopy()).crossProduct((positions[2]-positions[0]).normalisedCopy());
    }

    axisAlignedBox getAxisAlignedBoundingBox() const;

    plane getPlane() const;

    vector3 midPoint()
    {
        return (positions[0] + positions[1] + positions[2]) / 3.0;
    }

    vector3 positions[3];
};


}

#endif // TRIANGLEVECTOR3_HPP
