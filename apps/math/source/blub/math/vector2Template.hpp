#ifndef VECTOR2TEMPLATE_HPP
#define VECTOR2TEMPLATE_HPP


namespace blub
{

template <typename valueType, valueType valueDefault = 0>
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

    // not possible because of hash && multiple linking options
    // vector2Template(const valueType& value)

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

    /*vector2Template(const t_thisClass& copy)
        : x(copy.x)
        , y(copy.y)
    {
        ;
    }*/

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

    valueType x;
    valueType y;
};

}


#endif // VECTOR2TEMPLATE_HPP
