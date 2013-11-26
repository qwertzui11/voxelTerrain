#ifndef SERIALIZATION_CALLBASEOBJECT_HPP
#define SERIALIZATION_CALLBASEOBJECT_HPP

#include "blub/core/globals.hpp"

#include <boost/serialization/base_object.hpp>


namespace blub
{
namespace serialization
{


#if defined(__BORLANDC__) && __BORLANDC__ < 0x610
#error compiler not supported
#else
template<class Base, class Derived>
BOOST_DEDUCED_TYPENAME boost::serialization::detail::base_cast<Base, Derived>::type &
callBaseObject(Derived &d)
{
    return boost::serialization::base_object<Base>(d);
}
#endif


}
}




#endif // SERIALIZATION_CALLBASEOBJECT_HPP
