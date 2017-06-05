#ifndef BLUB_ARRAY_HPP
#define BLUB_ARRAY_HPP

#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"
#include "blub/serialization/saveLoad.hpp"

#include <array>

#include <boost/serialization/array.hpp>


namespace blub
{


template <class T, std::size_t N>
class array : public std::array<T, N>
{
public:
    typedef std::array<T, N> t_base;


private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        (void)version;

        t_base& casted = *this;
        readWrite & serialization::nameValuePair::create("elems", casted);
    }

};


}

#endif // BLUB_ARRAY_HPP
