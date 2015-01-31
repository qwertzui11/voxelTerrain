#ifndef BLUB_CORE_BYTEARRAY_HPP
#define BLUB_CORE_BYTEARRAY_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

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
     * @param length if length is smaller zero, the method returns an array from index to the end
     * @return
     */
    byteArray mid(const uint32& index, const int32& length = -1) const;
    uint32 size() const
    {
        return static_cast<uint32>(t_base::size());
    }

    byteArray& operator +=(const byteArray& other);

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & ar, const uint32& version)
    {
        (void)version;

        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(t_base);
    }

};

}

#endif // BLUB_CORE_BYTEARRAY_HPP
