#include "blub/core/byteArray.hpp"

#include "blub/core/string.hpp"

#include <cstring>


using namespace blub;


byteArray::byteArray(const char* str)
    : t_base(strlen(str))
{
    strcpy(t_base::data(), str);
}

byteArray::byteArray(const byteArray &array)
    : t_base(array.size())
{
    memcpy(t_base::data(), array.data(), array.size());
}

byteArray::byteArray(const char *array, uint32 size)
    : t_base(size)
{
    memcpy(t_base::data(), array, size);
}

byteArray::byteArray(const string &str)
    : t_base(str.size())
{
    memcpy(t_base::data(), str.data(), str.size());
}

byteArray byteArray::mid(const uint32 &index, const int32 &length) const
{
    int32 len(length);
    if (length < 0)
    {
        len = t_base::size()-index;
    }
    BASSERT(len >= 0);

    const byteArray result(t_base::data()+index, len);
    return result;
}

byteArray &byteArray::operator +=(const byteArray &other)
{
    const uint32 oldSize(t_base::size());
    t_base::resize(oldSize + other.size());
    memcpy(t_base::data()+oldSize, other.data(), other.size());

    return *this;
}
