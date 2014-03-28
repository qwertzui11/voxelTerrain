#include "global.hpp"

#include "blub/core/string.hpp"


using namespace blub::math::log;
using namespace blub;


BLUB_LOG_GLOBAL_LOGGER_INIT(global, blub::log::logger)
{
    blub::log::logger result("math");
    return result;
}


