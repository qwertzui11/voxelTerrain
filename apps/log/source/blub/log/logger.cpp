#include "logger.hpp"

#include "blub/core/string.hpp"

#include <boost/log/attributes/constant.hpp>


using namespace blub::log;


logger::logger(const blub::string &moduleName)
{
    t_base::add_attribute("Module", boost::log::attributes::constant<blub::string>(moduleName));
}


std::ostream &blub::log::operator<<(std::ostream &strm, blub::log::severity severity_)
{
    static const char* strings[] =
    {
        "out",
        "warning",
        "error",
    };
    std::size_t castedLevel(static_cast< std::size_t >(severity_));
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
