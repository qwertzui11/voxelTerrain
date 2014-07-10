#ifndef SERIALIZATION_ACCESS_HPP
#define SERIALIZATION_ACCESS_HPP

#include "blub/core/globals.hpp"

#include <boost/serialization/access.hpp>


namespace blub
{
namespace serialization
{


// typedef boost::serialization::access access;
#define BLUB_SERIALIZATION_ACCESS friend class boost::serialization::access;


}
}


#endif // SERIALIZATION_ACCESS_HPP
