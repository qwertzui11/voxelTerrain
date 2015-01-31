#ifndef BLUB_CORE_OPTIONAL_HPP
#define BLUB_CORE_OPTIONAL_HPP

#include <boost/optional.hpp>


namespace blub
{


template <class T>
using optional = boost::optional<T>;


}


#endif // BLUB_CORE_OPTIONAL_HPP
