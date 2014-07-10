#ifndef BLUB_MATH_VECTOR2TEMPLATE_HPP
#define BLUB_MATH_VECTOR2TEMPLATE_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"

#include <boost/functional/hash.hpp>


namespace blub
{

template <typename valueType, valueType valueDefault>
class vector2Template
{
public:
    typedef vector2Template<valueType, valueDefault> t_thisClass;

    vector2Template()
        : x(valueDefault)
        , y(valueDefault)
    {
        ;
    }

    vector2Template(const valueType& x_, const valueType& y_)
        : x(x_)
        , y(y_)
    {
        ;
    }

    vector2Template(const valueType& val)
        : x(val)
        , y(val)
    {
        ;
    }

    vector2Template(const t_thisClass& copy)
        : x(copy.x)
        , y(copy.y)
    {
        ;
    }

    bool operator == (const t_thisClass& other) const
    {
        return other.x == x && other.y == y;
    }

    bool operator <= (const t_thisClass& other) const
    {
        return x <= other.x && y <= other.y;
    }

    bool operator < (const t_thisClass& other) const
    {
        return x < other.x && y < other.y;
    }

    bool operator >= (const t_thisClass& other) const
    {
        return x >= other.x && y >= other.y;
    }

    bool operator > (const t_thisClass& other) const
    {
        return x > other.x && y > other.y;
    }

    t_thisClass operator * (const t_thisClass& other) const
    {
        return t_thisClass(other.x*x, other.y*y);
    }

    t_thisClass operator * (const int32& other) const
    {
        return t_thisClass(other*x, other*y);
    }

    t_thisClass operator + (const t_thisClass& other) const
    {
        return t_thisClass(other.x+x, other.y+y);
    }

    t_thisClass operator - (const t_thisClass& other) const
    {
        return t_thisClass(x-other.x, y-other.y);
    }

    t_thisClass operator - (void) const
    {
        return t_thisClass(-x, -y);
    }

    t_thisClass operator / (const t_thisClass& other) const
    {
        return t_thisClass(x/other.x, y/other.y);
    }

    t_thisClass operator / (const valueType& other) const
    {
        return t_thisClass(x/other, y/other);
    }

    valueType calculateArea() const
    {
        return x*y;
    }

protected:
    BLUB_SERIALIZATION_ACCESS
    template<typename Archive>
    void serialize(Archive & readWrite, const unsigned int version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
    }

public:
    valueType x;
    valueType y;

};

template <typename valueType, valueType valueDefault>
std::ostream& operator<< (std::ostream& ostr, const vector2Template<valueType, valueDefault>& toCast)
{
    return ostr << "(" << toCast.x << "," << toCast.y << ")";
}

template <typename valueType, valueType valueDefault>
std::size_t hash_value(const vector2Template<valueType, valueDefault>& value)
{
    std::size_t result(value.x);
    boost::hash_combine(result, value.y);

    return result;
}


}


#endif // BLUB_MATH_VECTOR2TEMPLATE_HPP
