#ifndef BLUB_LOG_GLOBAL_HPP
#define BLUB_LOG_GLOBAL_HPP

#include "blub/log/globalLogger.hpp"
#include "blub/log/logger.hpp"
#include "blub/log/predecl.hpp"


namespace blub
{
namespace log
{
BLUB_LOG_GLOBAL_LOGGER(global, logger)
}
}


#define BLUB_LOG_OUT_TO(destination) BOOST_LOG_SEV(destination, blub::log::severity::out)
#define BLUB_LOG_OUT() BLUB_LOG_OUT_TO(blub::log::global::get())

#define BLUB_LOG_WARNING_TO(destination) BOOST_LOG_SEV(destination, blub::log::severity::warning)
#define BLUB_LOG_WARNING() BLUB_LOG_WARNING_TO(blub::log::global::get())

#define BLUB_LOG_ERROR_TO(destination) BOOST_LOG_SEV(destination, blub::log::severity::error)
#define BLUB_LOG_ERROR() BLUB_LOG_ERROR_TO(blub::log::global::get())


#endif // BLUB_LOG_GLOBAL_HPP
