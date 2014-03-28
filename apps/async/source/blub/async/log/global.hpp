#ifndef BLUB_ASYNC_LOG_GLOBAL_HPP
#define BLUB_ASYNC_LOG_GLOBAL_HPP

#include "blub/log/global.hpp"
#include "blub/log/globalLogger.hpp"
#include "blub/log/logger.hpp"
#include "blub/log/predecl.hpp"


namespace blub
{
namespace async
{
namespace log
{
BLUB_LOG_GLOBAL_LOGGER(global, blub::log::logger)
}
}
}


#define BLUB_ASYNC_LOG_OUT() BLUB_LOG_OUT_TO(blub::async::log::global::get())
#define BLUB_ASYNC_LOG_WARNING() BLUB_LOG_WARNING_TO(blub::async::log::global::get())
#define BLUB_ASYNC_LOG_ERROR() BLUB_LOG_ERROR_TO(blub::async::log::global::get())


#endif // BLUB_ASYNC_LOG_GLOBAL_HPP
