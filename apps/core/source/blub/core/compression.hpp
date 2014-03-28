#ifndef COMPRESSION_HPP
#define COMPRESSION_HPP

#include "blub/core/globals.hpp"


namespace blub
{


class compression
{
public:
    static byteArray compressBz2(const byteArray &work);
    static byteArray decompressBz2(const byteArray &work);

};


}


#endif // COMPRESSION_HPP
