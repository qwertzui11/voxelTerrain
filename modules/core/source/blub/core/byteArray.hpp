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

    byteArray(const byteArray &array);
    byteArray(const char *array, uint32 size);
    byteArray(const string &str);

    /**
     * @brief mid gets an copy from a specified index for a length
     * @param index
     * @param length if length is smaller zero, the methoth returns an array from index to the end
     * @return
     */
    byteArray mid(const uint32& index, const int32& length) const;
    uint32 size() const
    {
        return static_cast<uint32>(t_base::size());
    }

    byteArray& operator +=(const byteArray& other);

};

}

#endif // BYTEARRAY_HPP
