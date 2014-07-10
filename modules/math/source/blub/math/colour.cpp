#include "blub/math/colour.hpp"

#ifndef BLUB_NO_OGRE3D
#include <OGRE/OgreColourValue.h>
#endif


using namespace blub;


#ifndef BLUB_NO_OGRE3D
colour::colour(const Ogre::ColourValue &vec)
    : r(vec.r)
    , g(vec.g)
    , b(vec.b)
    , a(vec.a)
{
    ;
}

colour::operator Ogre::ColourValue() const
{
    return Ogre::ColourValue(r, g, b, a);
}
#endif


