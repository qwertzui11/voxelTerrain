#include "transform.hpp"

#ifndef BLUB_NO_BULLET
#include <bullet/LinearMath/btTransform.h>
#endif

using namespace blub;

transform::transform(const blub::vector3 &position_, const blub::quaternion &rotation_, const blub::vector3 &scale_)
    : position(position_)
    , rotation(rotation_)
    , scale(scale_)
{
    ;
}

/*blub::transform::transform(const blub::transform &other)
    : position(other.position)
    , rotation(other.rotation)
    , scale(other.scale)
{
}*/

#ifndef BLUB_NO_BULLET
transform::transform(const btTransform &other)
    : position(other.getOrigin())
    , rotation(other.getRotation())
    , scale(1.)
{
}
transform::operator btTransform () const
{
    return btTransform(rotation, position);
}
#endif

bool transform::operator ==(const transform &other) const
{
    return position == other.position &&
            rotation == other.rotation &&
            scale == other.scale;
}

bool transform::operator !=(const transform &other) const
{
    return  position != other.position ||
            rotation != other.rotation ||
            scale != other.scale;
}

transform transform::operator +(const transform &other) const
{
    return transform(position+(rotation*other.position), rotation*other.rotation);
}

transform transform::getRelativeTo(const transform &other) const
{
    transform result;
    blub::vector3 buffer(other.position - position);
    buffer = (rotation.Inverse())*buffer;
    result.position = buffer;
    result.rotation = rotation.Inverse()*other.rotation;

    return result;
}
