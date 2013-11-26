#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"
#include "blub/serialization/saveLoad.hpp"

#include <boost/array.hpp>
#include <boost/serialization/array.hpp>


namespace blub
{


template <class T, std::size_t N>
class array : public boost::array<T, N>
{
public:
    typedef boost::array<T, N> t_base;


private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        (void)version;

        boost::serialization::array<T> toSerialize(t_base::data(), t_base::size());
        readWrite & serialization::nameValuePair::create("elems", toSerialize);
        // toSerialize.serialize(readWrite, version);
        // boost::serialization::serialize<formatType, T, N>(readWrite, static_cast<t_base>(*this), version);
        // serialization::callBaseObject<t_base>(*this);
    }

};


}

#endif // ARRAY_HPP
