#include "blub/core/string.hpp"

#include "blub/core/byteArray.hpp"

#include <ostream>


/*#ifndef NO_OGRE3D
#include <OGRE/OgreString.h>
#endif*/


using namespace blub;


string::string(const byteArray &array)
    : t_base(array.data(), array.size())
{

}


std::ostream &operator <<(std::ostream &ostr, const string &toCast)
{
    return ostr << static_cast<std::string>(toCast);
}
