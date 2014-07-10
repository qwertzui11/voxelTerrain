#ifndef VECTOR3TEMPLATE_HPP
#define VECTOR3TEMPLATE_HPP

#include "blub/math/math.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace blub
{

template <typename valueType, valueType valueDefault = 0>
class vector3Template
{
public:
    typedef valueType t_valueType;
    typedef vector3Template<t_valueType, valueDefault> t_thisClass;

    vector3Template()
        : x(valueDefault)
        , y(valueDefault)
        , z(valueDefault)
    {
        ;
    }

    // not possible because of hash && multiple linking options
    // vector3Template(const valueType& value)

    vector3Template(const valueType& x_, const valueType& y_, const valueType& z_)
        : x(x_)
        , y(y_)
        , z(z_)
    {
        ;
    }

    /*vector3Template(const t_thisClass& copy)
        : x(copy.x)
        , y(copy.y)
        , z(copy.z)
    {
        ;
    }*/

    vector3Template(const valueType& val)
        : x(val)
        , y(val)
        , z(val)
    {
        ;
    }

    bool operator == (const t_thisClass& other) const
    {
        return other.x == x && other.y == y && other.z == z;
    }

    bool operator != (const t_thisClass& other) const
    {
        return other.x != x && other.y != y && other.z != z;
    }

    bool operator <= (const t_thisClass& other) const
    {
        return x <= other.x && y <= other.y && z <= other.z;
    }

    bool operator < (const t_thisClass& other) const
    {
        return x < other.x && y < other.y && z < other.z;
    }

    bool operator >= (const t_thisClass& other) const
    {
        return x >= other.x && y >= other.y && z >= other.z;
    }

    bool operator > (const t_thisClass& other) const
    {
        return x > other.x && y > other.y && z > other.z;
    }

    t_thisClass operator * (const t_thisClass& other) const
    {
        return t_thisClass(other.x*x, other.y*y, other.z*z);
    }

    t_thisClass operator * (const int32& other) const
    {
        return t_thisClass(other*x, other*y, other*z);
    }

    t_thisClass operator + (const t_thisClass& other) const
    {
        return t_thisClass(other.x+x, other.y+y, other.z+z);
    }

    t_thisClass operator - (const t_thisClass& other) const
    {
        return t_thisClass(x-other.x, y-other.y, z-other.z);
    }

    t_thisClass operator - (void) const
    {
        return t_thisClass(-x, -y, -z);
    }

    t_thisClass operator / (const t_thisClass& other) const
    {
        return t_thisClass(x/other.x, y/other.y, z/other.z);
    }

    t_thisClass operator / (const valueType& other) const
    {
        return t_thisClass(x/other, y/other, z/other);
    }

    valueType operator [] (const blub::uint8& index) const
    {
        BASSERT(index < 3);
        return *((&x) + index);
    }

    t_thisClass getMinimum(const t_thisClass& other)
    {
        return t_thisClass(math::min(other.x, x), math::min(other.y, y), math::min(other.z, z));
    }

    t_thisClass getMaximum(const t_thisClass& other)
    {
        return t_thisClass(math::max(other.x, x), math::max(other.y, y), math::max(other.z, z));
    }


private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(z);
    }


public:
    valueType x;
    valueType y;
    valueType z;
};

}


#endif // VECTOR3TEMPLATE_HPP
