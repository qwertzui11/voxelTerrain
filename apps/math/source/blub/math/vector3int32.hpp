#ifndef VECTOR3INT32_HPP
#define VECTOR3INT32_HPP


#include "blub/core/globals.hpp"
#include "blub/math/vector3Template.hpp"


namespace blub
{

class vector3int32 : public vector3Template<int32, 0>
{
public:
    typedef vector3Template<int32> t_base;

    vector3int32()
    {
        ;
    }

    vector3int32(const int32& x, const int32& y, const int32& z)
        : t_base(x, y, z)
    {
        ;
    }
    /*depricated vector3int32(const vector3int32& copy)
        : t_base(copy)
    {
        ;
    }*/
    vector3int32(const t_base::t_thisClass& copy)
        : t_base(copy)
    {
        ;
    }
    vector3int32(const int32& val)
        : t_base(val)
    {
        ;
    }

    vector3int32(const vector3& cast);

    vector3int32 operator % (const vector3int32& other) const
    {
        return vector3int32(x % other.x, y % other.y, z % other.z);
    }
    vector3int32 operator % (const int32& other) const
    {
        return vector3int32(x % other, y % other, z % other);
    }

};

std::size_t hash_value(const vector3int32& value);

}


#endif // VECTOR3INT32_HPP
