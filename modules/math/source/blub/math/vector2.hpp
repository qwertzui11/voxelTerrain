#ifndef BLUB_VECTOR2_HPP
#define BLUB_VECTOR2_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace Ogre
{
    class Vector2;
}


namespace blub
{

class vector2
{
public:
#ifndef BLUB_NO_OGRE3D
    vector2(const Ogre::Vector2 &vec);
    operator Ogre::Vector2() const;
#endif

    vector2()
        : x(0.)
        , y(0.)
    {
    }

    vector2(const real& fX, const real& fY)
        : x(fX)
        , y(fY)
    {
    }

    vector2(const real& scaler)
        : x(scaler)
        , y(scaler)
    {
    }

    vector2(const vector2int32& cast);


    /** Assigns the value of the other vector.
        @param
            rkVector The other vector
    depricated
    inline vector2& operator = ( const vector2& rkVector )
    {
        x = rkVector.x;
        y = rkVector.y;

        return *this;
    }*/

    inline vector2& operator = ( const real fScalar)
    {
        x = fScalar;
        y = fScalar;

        return *this;
    }

    inline bool operator == ( const vector2& rkVector ) const
    {
        return ( x == rkVector.x && y == rkVector.y );
    }

    inline bool operator != ( const vector2& rkVector ) const
    {
        return ( x != rkVector.x || y != rkVector.y  );
    }

    // arithmetic operations
    inline vector2 operator + ( const vector2& rkVector ) const
    {
        return vector2(
            x + rkVector.x,
            y + rkVector.y);
    }

    inline vector2 operator - ( const vector2& rkVector ) const
    {
        return vector2(
            x - rkVector.x,
            y - rkVector.y);
    }

    inline vector2 operator * ( const real fScalar ) const
    {
        return vector2(
            x * fScalar,
            y * fScalar);
    }

    inline vector2 operator * ( const vector2& rhs) const
    {
        return vector2(
            x * rhs.x,
            y * rhs.y);
    }

    inline vector2 operator / ( const real fScalar ) const
    {
        BASSERT( fScalar != 0.0 );

        real fInv = 1.0f / fScalar;

        return vector2(
            x * fInv,
            y * fInv);
    }

    inline vector2 operator / ( const vector2& rhs) const
    {
        return vector2(
            x / rhs.x,
            y / rhs.y);
    }

    inline const vector2& operator + () const
    {
        return *this;
    }

    inline vector2 operator - () const
    {
        return vector2(-x, -y);
    }

    // overloaded operators to help vector2
    inline friend vector2 operator * ( const real fScalar, const vector2& rkVector )
    {
        return vector2(
            fScalar * rkVector.x,
            fScalar * rkVector.y);
    }

    inline friend vector2 operator / ( const real fScalar, const vector2& rkVector )
    {
        return vector2(
            fScalar / rkVector.x,
            fScalar / rkVector.y);
    }

    inline friend vector2 operator + (const vector2& lhs, const real rhs)
    {
        return vector2(
            lhs.x + rhs,
            lhs.y + rhs);
    }

    inline friend vector2 operator + (const real lhs, const vector2& rhs)
    {
        return vector2(
            lhs + rhs.x,
            lhs + rhs.y);
    }

    inline friend vector2 operator - (const vector2& lhs, const real rhs)
    {
        return vector2(
            lhs.x - rhs,
            lhs.y - rhs);
    }

    inline friend vector2 operator - (const real lhs, const vector2& rhs)
    {
        return vector2(
            lhs - rhs.x,
            lhs - rhs.y);
    }

    // arithmetic updates
    inline vector2& operator += ( const vector2& rkVector )
    {
        x += rkVector.x;
        y += rkVector.y;

        return *this;
    }

    inline vector2& operator += ( const real fScaler )
    {
        x += fScaler;
        y += fScaler;

        return *this;
    }

    inline vector2& operator -= ( const vector2& rkVector )
    {
        x -= rkVector.x;
        y -= rkVector.y;

        return *this;
    }

    inline vector2& operator -= ( const real fScaler )
    {
        x -= fScaler;
        y -= fScaler;

        return *this;
    }

    inline vector2& operator *= ( const real fScalar )
    {
        x *= fScalar;
        y *= fScalar;

        return *this;
    }

    inline vector2& operator *= ( const vector2& rkVector )
    {
        x *= rkVector.x;
        y *= rkVector.y;

        return *this;
    }

    inline vector2& operator /= ( const real fScalar )
    {
        BASSERT( fScalar != 0.0 );

        real fInv = 1.0f / fScalar;

        x *= fInv;
        y *= fInv;

        return *this;
    }

    inline vector2& operator /= ( const vector2& rkVector )
    {
        x /= rkVector.x;
        y /= rkVector.y;

        return *this;
    }

public:
    real x, y;

protected:
    BLUB_SERIALIZATION_ACCESS
    template<typename Archive>
    void serialize(Archive & readWrite, const unsigned int version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
    }

};

}

#endif // BLUB_VECTOR2_HPP
