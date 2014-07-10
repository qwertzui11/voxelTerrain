#ifndef COLOUR_HPP
#define COLOUR_HPP

#include "blub/core/globals.hpp"


namespace Ogre
{
    class ColourValue;
}


namespace blub
{


class colour
{
public:
#ifndef BLUB_NO_OGRE3D
    colour(const Ogre::ColourValue &vec);
    operator Ogre::ColourValue() const;
#endif

    colour()
        : r(0.)
        , g(0.)
        , b(0.)
        , a(0.)
    {;}
    colour(const colour& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
        , a(other.a)
    {;}
    colour(const real& r_, const real& g_, const real& b_, const real& a_ = 1.)
        : r(r_)
        , g(g_)
        , b(b_)
        , a(a_)
    {;}

public:
    real r, g, b, a;

};


}

#endif // COLOUR_HPP
