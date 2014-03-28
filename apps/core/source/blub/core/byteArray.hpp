#ifndef BYTEARRAY_HPP
#define BYTEARRAY_HPP

#include "blub/core/globals.hpp"

#include <vector>


namespace blub
{

class byteArray : public std::vector<char>
{
public:
    typedef std::vector<char> t_base;

    byteArray()
    {
        ;
    }
    byteArray(const uint32& size)
        : t_base(size, 0)
    {;}

    byteArray(const char* str);
    byteArray(const byteArray &array);
    byteArray(const char *array, uint32 size);
    byteArray(const string &str);

    byteArray mid(const uint32& index, const int32& length) const;
    uint32 size() const
    {
        return (uint32)t_base::size();
    }

    byteArray& operator +=(const byteArray& other);

};

}

#endif // BYTEARRAY_HPP
