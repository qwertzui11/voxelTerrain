#ifndef BLUB_LOG_GLOBALLOGGER_HPP
#define BLUB_LOG_GLOBALLOGGER_HPP

#include <boost/log/sources/global_logger_storage.hpp>

#define BLUB_LOG_GLOBAL_LOGGER(name, logger_type) BOOST_LOG_GLOBAL_LOGGER(name, logger_type)
#define BLUB_LOG_GLOBAL_LOGGER_INIT(name, logger_type) BOOST_LOG_GLOBAL_LOGGER_INIT(name, logger_type)

#endif // BLUB_LOG_GLOBALLOGGER_HPP
