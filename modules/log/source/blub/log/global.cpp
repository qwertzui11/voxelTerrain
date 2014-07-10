#include "global.hpp"

#include "blub/core/string.hpp"
#include "blub/log/system.hpp"

#include <boost/log/utility/setup/common_attributes.hpp>


using namespace blub::log;


BLUB_LOG_GLOBAL_LOGGER_INIT(global, blub::log::logger)
{
    blub::log::system::addConsole();

    boost::log::add_common_attributes();

    logger result("application");
    return result;
}
