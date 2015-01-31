#include "blub/core/string.hpp"

#include "blub/core/byteArray.hpp"

#include <ostream>

#include <boost/functional/hash.hpp>


using namespace blub;


string::string(const byteArray &array)
    : t_base(array.data(), array.size())
{

}


std::ostream &operator <<(std::ostream &ostr, const string &toCast)
{
    return ostr << static_cast<std::string>(toCast);
}


std::size_t blub::hash_value(const string &value)
{
    std::size_t result(boost::hash_value<std::string>(static_cast<std::string>(value)));

    return result;
}

