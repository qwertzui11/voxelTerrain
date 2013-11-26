#include "blub/math/vector2.hpp"
#include "blub/math/vector2int32.hpp"

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
