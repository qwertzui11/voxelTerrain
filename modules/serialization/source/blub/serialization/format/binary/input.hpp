#ifndef SERIALIZATION_FORMAT_BINARY_INPUT_HPP
#define SERIALIZATION_FORMAT_BINARY_INPUT_HPP

#include "blub/core/globals.hpp"

#include "blub/serialization/format/binary/portable_binary_iarchive.hpp"
//#include <boost/archive/binary_iarchive.hpp>


namespace blub
{
namespace serialization
{
namespace format
{
namespace binary
{


typedef portable_binary_iarchive input;
//typedef boost::archive::binary_iarchive input;


}
}
}
}


#endif // SERIALIZATION_FORMAT_BINARY_INPUT_HPP
