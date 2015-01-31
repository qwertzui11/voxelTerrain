#include "system.hpp"

#include "blub/log/logger.hpp"
#include "blub/core/string.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/support/date_time.hpp>

#include <iostream>


using namespace blub::log;
using namespace blub;



struct severity_tag;

boost::log::formatting_ostream& operator<<
(
    boost::log::formatting_ostream& strm,
    boost::log::to_log_manip< blub::log::severity, severity_tag > const& manip
)
{
    static const char* strings[] =
    {
        "info",
        "warn",
        "err",
    };

    blub::log::severity level = manip.get();
    std::size_t castedLevel(static_cast< std::size_t >(level));
    if (castedLevel < sizeof(strings) / sizeof(*strings))
    {
        strm << strings[castedLevel];
    }
    else
    {
        strm << castedLevel;
    }

    return strm;
}


void system::addFile(const string &file)
{
    boost::log::add_file_log(
                static_cast<std::string>(file),
                boost::log::keywords::format =
                        (
                            boost::log::expressions::stream
                                << boost::log::expressions::attr< severity, severity_tag >("Severity")
                                << " " << boost::log::expressions::attr< boost::posix_time::ptime >("TimeStamp")
                                << " " << boost::log::expressions::attr< boost::log::attributes::current_thread_id::value_type >("ThreadID")
                                << " " << boost::log::expressions::attr< blub::string >("Module")
#ifdef BLUB_LOG_LOCATION
                                << " " << boost::log::expressions::attr< blub::string >("Location")
                                << "\n"
#else
                                << " "
#endif
                                << boost::log::expressions::smessage
                        ),
                boost::log::keywords::auto_flush = true
        );
}

void system::addConsole()
{
    boost::log::add_console_log(
                std::cout,
                boost::log::keywords::format =
                        (
                            boost::log::expressions::stream
                                << boost::log::expressions::attr<severity, severity_tag>("Severity")
                                << " " << boost::log::expressions::attr<boost::posix_time::ptime>("TimeStamp")
                                << " " << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
                                << " " << boost::log::expressions::attr< blub::string >("Module")
#ifdef BLUB_LOG_LOCATION
                                << " " << boost::log::expressions::attr< blub::string >("Location")
                                << "\n"
#else
                                << " "
#endif
                                << boost::log::expressions::smessage
                        ),
                boost::log::keywords::auto_flush = true
        );
}
