#ifndef BLUB_LOG_LOGGER_HPP
#define BLUB_LOG_LOGGER_HPP

#include "blub/core/globals.hpp"

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <ostream>


namespace blub
{
namespace log
{
    enum class severity
    {
        out,
        warning,
        error
    }; 

    std::ostream& operator<< (std::ostream& strm, blub::log::severity severity_);

    class logger : public boost::log::sources::severity_logger_mt<blub::log::severity>
    {
    public:
        typedef boost::log::sources::severity_logger_mt<blub::log::severity> t_base;

        logger(const blub::string &moduleName);

    };
}
}


#endif // BLUB_LOG_LOGGER_HPP
