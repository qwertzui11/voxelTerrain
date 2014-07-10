#include "blub/math/vector2.hpp"
#include "blub/math/vector2int.hpp"

#include <ostream>

#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgreVector2.h>
#endif


using namespace blub;


#ifndef BLUB_NO_OGRE3D
vector2::vector2(const Ogre::Vector2 &vec)
    : x(vec.x)
    , y(vec.y)
{

}

vector2::operator Ogre::Vector2() const
{
    return Ogre::Vector2(x, y);
}

#endif


vector2::vector2(const vector2int32& cast)
    : x(cast.x)
    , y(cast.y)
{
}


std::ostream &blub::operator<< (std::ostream &ostr, const vector2 &toCast)
{
    return ostr << "(" << toCast.x << "," << toCast.y << ")";
}


std::size_t blub::hash_value(const vector2 &value)
{
    std::size_t result(value.x);
    boost::hash_combine(result, value.y);

    return result;
}
