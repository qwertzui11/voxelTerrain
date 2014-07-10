#ifndef BLUBLOG_HPP
#define BLUBLOG_HPP

#include "blub/core/globals.hpp"
#include "blub/core/string.hpp"

#include "blub/log/global.hpp"

#include <fstream>


namespace blub
{

namespace log
{

    void doIt(const string &out);

}


#define BLUB_LOG(x) log::doIt(x)
#define BERROR(x) BLUB_LOG(x)
#define BWARNING(x) BLUB_LOG(x)
#define BOUT(x) BLUB_LOG(x)


}


#endif // BLUBLOG_HPP
