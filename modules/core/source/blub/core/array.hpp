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

        boost::serialization::array<T> toSerialize(t_base::data(), t_base::size());
        readWrite & serialization::nameValuePair::create("elems", toSerialize);
    }

};


}

#endif // BLUB_ARRAY_HPP
