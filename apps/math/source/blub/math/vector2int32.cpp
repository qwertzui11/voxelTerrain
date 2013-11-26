#include "vector2int32.hpp"


#include "blub/math/vector2.hpp"

#include <boost/functional/hash.hpp>


std::size_t blub::hash_value(const blub::vector2int32& value)
{
    std::size_t result(value.x);
    boost::hash_combine(result, value.y);

    return result;
}


blub::vector2int32::vector2int32(const blub::vector2 &cast)
{
    x = (int32)cast.x;
    y = (int32)cast.y;
}
