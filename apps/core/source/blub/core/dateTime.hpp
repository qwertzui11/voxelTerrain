#ifndef DATETIME_HPP
#define DATETIME_HPP

#include "blub/core/string.hpp"


namespace blub
{

class dateTime
{
public:
    // converts to YYYY-mmm-DD HH:MM::SS.ffffff / YYYY-mmm-DD HH:MM::SS.fffffff
    static string getUniversalTimeString();

};

}

#endif // DATETIME_HPP
