#include "log.hpp"

#include "blub/log/global.hpp"


void blub::log::doIt(const blub::string &out)
{
    BLUB_LOG_OUT() << out;
}
