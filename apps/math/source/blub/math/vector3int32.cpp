#include "blub/math/vector3int32.hpp"

#include "blub/math/vector3.hpp"

#include <boost/functional/hash.hpp>
#include <ostream>


blub::vector3int32::vector3int32(const blub::vector3 &cast)
{
    x = cast.x;
    y = cast.y;
    z = cast.z;
}

std::ostream &blub::operator<<(std::ostream &os, const blub::vector3int32 &toCast)
{
    return os << "(" << toCast.x << "," << toCast.y << "," << toCast.z << ")";
}

std::size_t blub::hash_value(const blub::vector3int32& value)
{
    std::size_t result(value.x);
    boost::hash_combine(result, value.y);
    boost::hash_combine(result, value.z);

    return result;
}
