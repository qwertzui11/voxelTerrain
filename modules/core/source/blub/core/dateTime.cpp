#include "blub/core/dateTime.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>


using namespace blub;


string dateTime::getUniversalTimeString()
{
    const boost::posix_time::ptime timestamp(boost::posix_time::microsec_clock::universal_time());
    return boost::posix_time::to_simple_string(timestamp);
}



