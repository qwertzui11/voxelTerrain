#ifndef BLUB_CORE_BASE64_HPP
#define BLUB_CORE_BASE64_HPP

#include "blub/core/globals.hpp"


namespace blub
{


class base64
{
public:
    static string encode(const byteArray &work);
    static byteArray decode(const string &work);

};


}


#endif // BLUB_CORE_BASE64_HPP
