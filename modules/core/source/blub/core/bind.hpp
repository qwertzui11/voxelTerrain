#ifndef BLUB_CORE_BIND_HPP
#define BLUB_CORE_BIND_HPP

#include <boost/config.hpp>

#ifdef BOOST_NO_CXX11_HDR_FUNCTIONAL
#   include <boost/bind.hpp>
#   include <boost/function.hpp>
#else
#   include <functional>
#endif


namespace blub
{


#ifdef BOOST_NO_CXX11_HDR_FUNCTIONAL
using boost::bind;
namespace placeholders {
    using ::_1;
    using ::_2;
}
#else
using std::bind;
namespace placeholders = std::placeholders;
#endif


}



#endif // BLUB_CORE_BIND_HPP
