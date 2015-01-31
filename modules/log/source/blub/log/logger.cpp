#include "logger.hpp"

#include "blub/core/string.hpp"

#include <boost/log/attributes/constant.hpp>


using namespace blub::log;


logger::logger(const blub::string &moduleName)
{
    t_base::add_attribute("Module", boost::log::attributes::constant<blub::string>(moduleName));
}


