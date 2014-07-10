#include "triangleVector3.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/plane.hpp"


using namespace blub;


axisAlignedBox triangleVector3::getAxisAlignedBoundingBox() const
{
    vector3 min(positions[0]);
    min.makeFloor(positions[1]);
    min.makeFloor(positions[2]);

    vector3 max(positions[0]);
    max.makeCeil(positions[1]);
    max.makeCeil(positions[2]);

    return axisAlignedBox(min, max);
}

plane triangleVector3::getPlane() const
{
    return plane(positions[0], positions[1], positions[2]);
}
