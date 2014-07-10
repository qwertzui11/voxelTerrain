#ifndef VECTOR2INT32_HPP
#define VECTOR2INT32_HPP


#include "blub/core/globals.hpp"
#include "blub/math/vector2Template.hpp"


namespace blub
{

class vector2int32 : public vector2Template<int32, 0>
{
public:
    typedef vector2Template<int32> t_base;

    vector2int32()
        : t_base()
    {
        ;
    }

    vector2int32(const int32& x, const int32& y)
        : t_base(x, y)
    {
        ;
    }
    /*depricated vector2int32(const vector2int32& copy)
        : t_base(copy)
    {
        ;
    }*/
    vector2int32(const t_base::t_thisClass& copy)
        : t_base(copy)
    {
        ;
    }

    vector2int32(const vector2& cast);

protected:
    BLUB_SERIALIZATION_ACCESS
    template<typename Archive>
    void serialize(Archive & readWrite, const unsigned int version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
    }
};

std::size_t hash_value(const vector2int32& value);

}


#endif // VECTOR2INT32_HPP
