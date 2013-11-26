#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "blub/core/globals.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/quaternion.hpp"

class btTransform;

namespace blub
{

class transform
{
public:
    transform(const vector3& position_ = vector3(), const quaternion& rotation_ = quaternion(), const vector3& scale_ = vector3(1.));
    //depricated transform(const transform& other);

#ifndef BLUB_NO_BULLET
    transform(const btTransform& other);
    operator btTransform() const;
#endif

    bool operator ==(const transform& other) const;
    bool operator !=(const transform& other) const;

    transform operator +(const transform& other) const;

    transform getRelativeTo(const transform& other) const;

public:
    vector3 position;
    quaternion rotation;
    vector3 scale;
};

}

#endif // TRANSFORM_HPP
