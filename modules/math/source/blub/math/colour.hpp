#ifndef BLUB_MATH_COLOUR_HPP
#define BLUB_MATH_COLOUR_HPP

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
        , a(1.)
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

    bool operator ==(const colour& toCompare) const
    {
        return toCompare.a == a &&
                toCompare.b == b &&
                toCompare.g == g &&
                toCompare.r == r;
    }
    bool operator !=(const colour& toCompare) const
    {
        return toCompare.a != a ||
                toCompare.b != b ||
                toCompare.g != g ||
                toCompare.r != r;
    }
    inline colour operator + (const colour& toAdd) const
    {
        colour kSum;

        kSum.r = r + toAdd.r;
        kSum.g = g + toAdd.g;
        kSum.b = b + toAdd.b;
        kSum.a = a + toAdd.a;

        return kSum;
    }
    inline colour operator - (const colour& toSub) const
    {
        colour kDiff;

        kDiff.r = r - toSub.r;
        kDiff.g = g - toSub.g;
        kDiff.b = b - toSub.b;
        kDiff.a = a - toSub.a;

        return kDiff;
    }
    inline colour operator / (const real scalar) const
    {
        BASSERT(scalar != 0.0);

        colour kDiv;

        real fInv = 1.0f / scalar;
        kDiv.r = r * fInv;
        kDiv.g = g * fInv;
        kDiv.b = b * fInv;
        kDiv.a = a * fInv;

        return kDiv;
    }
public:
    real r, g, b, a;

};


std::ostream& operator<< (std::ostream& ostr, const colour& toCast);


}

#endif // BLUB_MATH_COLOUR_HPP
