#ifndef CLASSVERSION_HPP
#define CLASSVERSION_HPP

#include "blub/core/globals.hpp"

#include <boost/serialization/version.hpp>


#define BLUB_CLASSVERSION(T, N) BOOST_CLASS_VERSION(T, N)


#endif // CLASSVERSION_HPP
