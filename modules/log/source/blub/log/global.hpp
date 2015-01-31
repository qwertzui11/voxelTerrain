#ifndef BLUB_LOG_GLOBAL_HPP
#define BLUB_LOG_GLOBAL_HPP

#include "blub/core/string.hpp"
#include "blub/log/globalLogger.hpp"
#include "blub/log/logger.hpp"
#include "blub/log/predecl.hpp"

#include <boost/log/utility/manipulators/add_value.hpp>


namespace blub
{
namespace log
{
BLUB_LOG_GLOBAL_LOGGER(global, logger)
}
}


// TODO custom log source would be more beautiful http://www.boost.org/doc/libs/1_57_0/libs/log/doc/html/log/extension/sources.html
#ifdef BLUB_LOG_LOCATION
#   define BLUB_LOG_SEV(destination, severity) BOOST_LOG_SEV(destination, severity) << boost::log::add_value<blub::string>("Location", __PRETTY_FUNCTION__)
#   define BLUB_LOG_SEV_WITHOUT_LOCATION(destination, severity) BOOST_LOG_SEV(destination, severity) << boost::log::add_value<blub::string>("Location", __PRETTY_FUNCTION__)
#else
#   define BLUB_LOG_SEV(destination, severity) BOOST_LOG_SEV(destination, severity)
#   define BLUB_LOG_SEV_WITHOUT_LOCATION(destination, severity) BOOST_LOG_SEV(destination, severity)
#endif

#define BLUB_LOG_OUT_TO(destination) BLUB_LOG_SEV(destination, blub::log::severity::out)
#define BLUB_LOG_OUT() BLUB_LOG_OUT_TO(blub::log::global::get())

#define BLUB_LOG_WARNING_TO(destination) BLUB_LOG_SEV(destination, blub::log::severity::warning)
#define BLUB_LOG_WARNING() BLUB_LOG_WARNING_TO(blub::log::global::get())

#define BLUB_LOG_ERROR_TO(destination) BLUB_LOG_SEV(destination, blub::log::severity::error)
#define BLUB_LOG_ERROR() BLUB_LOG_ERROR_TO(blub::log::global::get())


#endif // BLUB_LOG_GLOBAL_HPP
