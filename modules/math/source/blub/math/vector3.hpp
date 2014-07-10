#ifndef BLUB_MATH_VECTOR3_HPP
#define BLUB_MATH_VECTOR3_HPP

#include "blub/core/classVersion.hpp"
#include "blub/core/globals.hpp"
#include "blub/math/math.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"
#ifdef BLUB_USE_PHYSX
#   include <foundation/PxVec3.h>
#endif

#ifdef BLUB_USE_BULLET
class btVector3;
#endif
namespace Ogre
{
    class Vector3;
}


namespace blub
{

class vector3
{
public:
    vector3()
        : x(0.)
        , y(0.)
        , z(0.)
    {
    }

#ifndef BLUB_NO_OGRE3D
    vector3(const Ogre::Vector3 &vec);
    operator Ogre::Vector3() const;
#endif
#ifdef BLUB_USE_PHYSX
    vector3(const physx::PxVec3& other)
        : x(other.x), y(other.y), z(other.z)
    {}
    operator physx::PxVec3() const
    {return physx::PxVec3(x,y,z);}
#endif
#ifndef BLUB_NO_BULLET
    vector3(const btVector3 &vec);
    operator btVector3() const;
#endif

    vector3(const real fX, const real fY, const real fZ)
        : x(fX)
        , y(fY)
        , z(fZ)
    {
    }

    vector3(const real& scaler)
        : x(scaler)
        , y(scaler)
        , z(scaler)
    {
    }

    vector3(const vector3int32& cast);

    bool operator <= ( const vector3& rhs ) const
    {
        return x <= rhs.x && y <= rhs.y && z <= rhs.z;
    }

    bool operator >= ( const vector3& rhs ) const
    {
        return x >= rhs.x && y >= rhs.y && z >= rhs.z;
    }

    vector3 getFloor(void) const;
    vector3 getAbs(void) const;


    /** Exchange the contents of this vector with another.
    */
    inline void swap(vector3& other)
    {
        std::swap(x, other.x);
        std::swap(y, other.y);
        std::swap(z, other.z);
    }

    inline real operator [] ( const size_t i ) const
    {
        BASSERT( i < 3 );

        return *(&x+i);
    }

    inline real& operator [] ( const size_t i )
    {
        BASSERT( i < 3 );

        return *(&x+i);
    }
    /// Pointer accessor for direct copying
    inline real* ptr()
    {
        return &x;
    }
    /// Pointer accessor for direct copying
    inline const real* ptr() const
    {
        return &x;
    }

    /** Assigns the value of the other vector.
        @param
            rkVector The other vector
    depricated!
    inline vector3& operator = ( const vector3& rkVector )
    {
        x = rkVector.x;
        y = rkVector.y;
        z = rkVector.z;

        return *this;
    }
    */

    inline vector3& operator = ( const real fScaler )
    {
        x = fScaler;
        y = fScaler;
        z = fScaler;

        return *this;
    }

    inline bool operator == ( const vector3& rkVector ) const
    {
        return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
    }

    inline bool operator != ( const vector3& rkVector ) const
    {
        return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
    }

    // arithmetic operations
    inline vector3 operator + ( const vector3& rkVector ) const
    {
        return vector3(
            x + rkVector.x,
            y + rkVector.y,
            z + rkVector.z);
    }

    inline vector3 operator - ( const vector3& rkVector ) const
    {
        return vector3(
            x - rkVector.x,
            y - rkVector.y,
            z - rkVector.z);
    }

    inline vector3 operator * ( const real fScalar ) const
    {
        return vector3(
            x * fScalar,
            y * fScalar,
            z * fScalar);
    }

    inline vector3 operator * ( const vector3& rhs) const
    {
        return vector3(
            x * rhs.x,
            y * rhs.y,
            z * rhs.z);
    }

    inline vector3 operator / ( const real fScalar ) const
    {
        BASSERT( fScalar != 0.0 );

        real fInv = 1.0f / fScalar;

        return vector3(
            x * fInv,
            y * fInv,
            z * fInv);
    }

    inline vector3 operator / ( const vector3& rhs) const
    {
        return vector3(
            x / rhs.x,
            y / rhs.y,
            z / rhs.z);
    }

    inline const vector3& operator + () const
    {
        return *this;
    }

    inline vector3 operator - () const
    {
        return vector3(-x, -y, -z);
    }

    // overloaded operators to help vector3
    inline friend vector3 operator * ( const real fScalar, const vector3& rkVector )
    {
        return vector3(
            fScalar * rkVector.x,
            fScalar * rkVector.y,
            fScalar * rkVector.z);
    }

    inline friend vector3 operator / ( const real fScalar, const vector3& rkVector )
    {
        return vector3(
            fScalar / rkVector.x,
            fScalar / rkVector.y,
            fScalar / rkVector.z);
    }

    inline friend vector3 operator + (const vector3& lhs, const real rhs)
    {
        return vector3(
            lhs.x + rhs,
            lhs.y + rhs,
            lhs.z + rhs);
    }

    inline friend vector3 operator + (const real lhs, const vector3& rhs)
    {
        return vector3(
            lhs + rhs.x,
            lhs + rhs.y,
            lhs + rhs.z);
    }

    inline friend vector3 operator - (const vector3& lhs, const real rhs)
    {
        return vector3(
            lhs.x - rhs,
            lhs.y - rhs,
            lhs.z - rhs);
    }

    inline friend vector3 operator - (const real lhs, const vector3& rhs)
    {
        return vector3(
            lhs - rhs.x,
            lhs - rhs.y,
            lhs - rhs.z);
    }

    // arithmetic updates
    inline vector3& operator += ( const vector3& rkVector )
    {
        x += rkVector.x;
        y += rkVector.y;
        z += rkVector.z;

        return *this;
    }

    inline vector3& operator += ( const real fScalar )
    {
        x += fScalar;
        y += fScalar;
        z += fScalar;
        return *this;
    }

    inline vector3& operator -= ( const vector3& rkVector )
    {
        x -= rkVector.x;
        y -= rkVector.y;
        z -= rkVector.z;

        return *this;
    }

    inline vector3& operator -= ( const real fScalar )
    {
        x -= fScalar;
        y -= fScalar;
        z -= fScalar;
        return *this;
    }

    inline vector3& operator *= ( const real fScalar )
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
        return *this;
    }

    inline vector3& operator *= ( const vector3& rkVector )
    {
        x *= rkVector.x;
        y *= rkVector.y;
        z *= rkVector.z;

        return *this;
    }

    inline vector3& operator /= ( const real fScalar )
    {
        BASSERT( fScalar != 0.0 );

        real fInv = 1.0f / fScalar;

        x *= fInv;
        y *= fInv;
        z *= fInv;

        return *this;
    }

    inline vector3& operator /= ( const vector3& rkVector )
    {
        x /= rkVector.x;
        y /= rkVector.y;
        z /= rkVector.z;

        return *this;
    }


    /** Returns the length (magnitude) of the vector.
        @warning
            This operation requires a square root and is expensive in
            terms of CPU operations. If you don't need to know the exact
            length (e.g. for just comparing lengths) use squaredLength()
            instead.
    */
    inline real length () const
    {
        return math::sqrt( x * x + y * y + z * z );
    }

    /** Returns the square of the length(magnitude) of the vector.
        @remarks
            This  method is for efficiency - calculating the actual
            length of a vector requires a square root, which is expensive
            in terms of the operations required. This method returns the
            square of the length of the vector, i.e. the same as the
            length but before the square root is taken. Use this if you
            want to find the longest / shortest vector without incurring
            the square root.
    */
    inline real squaredLength () const
    {
        return x * x + y * y + z * z;
    }

    /** Returns the distance to another vector.
        @warning
            This operation requires a square root and is expensive in
            terms of CPU operations. If you don't need to know the exact
            distance (e.g. for just comparing distances) use squaredDistance()
            instead.
    */
    inline real distance(const vector3& rhs) const
    {
        return (*this - rhs).length();
    }

    /** Returns the square of the distance to another vector.
        @remarks
            This method is for efficiency - calculating the actual
            distance to another vector requires a square root, which is
            expensive in terms of the operations required. This method
            returns the square of the distance to another vector, i.e.
            the same as the distance but before the square root is taken.
            Use this if you want to find the longest / shortest distance
            without incurring the square root.
    */
    inline real squaredDistance(const vector3& rhs) const
    {
        return (*this - rhs).squaredLength();
    }

    /** Calculates the dot (scalar) product of this vector with another.
        @remarks
            The dot product can be used to calculate the angle between 2
            vectors. If both are unit vectors, the dot product is the
            cosine of the angle; otherwise the dot product must be
            divided by the product of the lengths of both vectors to get
            the cosine of the angle. This result can further be used to
            calculate the distance of a point from a plane.
        @param
            vec Vector with which to calculate the dot product (together
            with this one).
        @return
            A float representing the dot product value.
    */
    inline real dotProduct(const vector3& vec) const
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    /** Calculates the absolute dot (scalar) product of this vector with another.
        @remarks
            This function work similar dotProduct, except it use absolute value
            of each component of the vector to computing.
        @param
            vec Vector with which to calculate the absolute dot product (together
            with this one).
        @return
            A real representing the absolute dot product value.
    */
    inline real absDotProduct(const vector3& vec) const
    {
        return math::abs(x * vec.x) + math::abs(y * vec.y) + math::abs(z * vec.z);
    }

    /** Normalises the vector.
        @remarks
            This method normalises the vector such that it's
            length / magnitude is 1. The result is called a unit vector.
        @note
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
        @return The previous length of the vector.
    */
    inline real normalise()
    {
        real fLength = math::sqrt( x * x + y * y + z * z );

        // Will also work for zero-sized vectors, but will change nothing
        // We're not using epsilons because we don't need to.
        // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
        if ( fLength > real(0.0f) )
        {
            real fInvLength = 1.0f / fLength;
            x *= fInvLength;
            y *= fInvLength;
            z *= fInvLength;
        }

        return fLength;
    }

    /** Calculates the cross-product of 2 vectors, i.e. the vector that
        lies perpendicular to them both.
        @remarks
            The cross-product is normally used to calculate the normal
            vector of a plane, by calculating the cross-product of 2
            non-equivalent vectors which lie on the plane (e.g. 2 edges
            of a triangle).
        @param rkVector
            Vector which, together with this one, will be used to
            calculate the cross-product.
        @return
            A vector which is the result of the cross-product. This
            vector will <b>NOT</b> be normalised, to maximise efficiency
            - call vector3::normalise on the result if you wish this to
            be done. As for which side the resultant vector will be on, the
            returned vector will be on the side from which the arc from 'this'
            to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
            = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
            This is because OGRE uses a right-handed coordinate system.
        @par
            For a clearer explanation, look a the left and the bottom edges
            of your monitor's screen. Assume that the first vector is the
            left edge and the second vector is the bottom edge, both of
            them starting from the lower-left corner of the screen. The
            resulting vector is going to be perpendicular to both of them
            and will go <i>inside</i> the screen, towards the cathode tube
            (assuming you're using a CRT monitor, of course).
    */
    inline vector3 crossProduct( const vector3& rkVector ) const
    {
        return vector3(
            y * rkVector.z - z * rkVector.y,
            z * rkVector.x - x * rkVector.z,
            x * rkVector.y - y * rkVector.x);
    }

    /** Returns a vector at a point half way between this and the passed
        in vector.
    */
    inline vector3 midPoint( const vector3& vec ) const
    {
        return vector3(
            ( x + vec.x ) * 0.5f,
            ( y + vec.y ) * 0.5f,
            ( z + vec.z ) * 0.5f );
    }

    /** Returns true if the vector's scalar components are all greater
        that the ones of the vector it is compared against.
    */
    inline bool operator < ( const vector3& rhs ) const
    {
        if( x < rhs.x && y < rhs.y && z < rhs.z )
            return true;
        return false;
    }

    /** Returns true if the vector's scalar components are all smaller
        that the ones of the vector it is compared against.
    */
    inline bool operator > ( const vector3& rhs ) const
    {
        if( x > rhs.x && y > rhs.y && z > rhs.z )
            return true;
        return false;
    }

    /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
            'Minimum' in this case means the combination of the lowest
            value of x, y and z from both vectors. Lowest is taken just
            numerically, not magnitude, so -1 < 0.
    */
    inline void makeFloor( const vector3& cmp )
    {
        if( cmp.x < x ) x = cmp.x;
        if( cmp.y < y ) y = cmp.y;
        if( cmp.z < z ) z = cmp.z;
    }

    /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
            'Maximum' in this case means the combination of the highest
            value of x, y and z from both vectors. Highest is taken just
            numerically, not magnitude, so 1 > -3.
    */
    inline void makeCeil( const vector3& cmp )
    {
        if( cmp.x > x ) x = cmp.x;
        if( cmp.y > y ) y = cmp.y;
        if( cmp.z > z ) z = cmp.z;
    }

    /** Generates a vector perpendicular to this vector (eg an 'up' vector).
        @remarks
            This method will return a vector which is perpendicular to this
            vector. There are an infinite number of possibilities but this
            method will guarantee to generate one of them. If you need more
            control you should use the quaternion class.
    */
    inline vector3 perpendicular(void) const
    {
        static const real fSquareZero = (real)(1e-06 * 1e-06);

        vector3 perp = this->crossProduct( vector3::UNIT_X );

        // Check length
        if( perp.squaredLength() < fSquareZero )
        {
            /* This vector is the Y axis multiplied by a scalar, so we have
               to use another axis.
            */
            perp = this->crossProduct( vector3::UNIT_Y );
        }
        perp.normalise();

        return perp;
    }


    /** Gets the angle between 2 vectors.
    @remarks
        Vectors do not have to be unit-length but must represent directions.
    */
    inline real angleBetween(const vector3& dest) const
    {
        real lenProduct = length() * dest.length();

        // Divide by zero check
        if(lenProduct < 1e-6f)
            lenProduct = 1e-6f;

        real f = dotProduct(dest) / lenProduct;

        f = math::clamp(f, (real)-1.0, (real)1.0);
        return math::acos(f);

    }
    /** Gets the shortest arc quaternion to rotate this vector to the destination
        vector.
    @remarks
        If you call this with a dest vector that is close to the inverse
        of this vector, we will rotate 180 degrees around the 'fallbackAxis'
        (if specified, or a generated axis if not) since in this case
        ANY axis of rotation is valid.
    */
    quaternion getRotationTo(const vector3& dest,
        const vector3& fallbackAxis = vector3::ZERO) const;

    /** Returns true if this vector is zero length. */
    inline bool isZeroLength(void) const
    {
        real sqlen = (x * x) + (y * y) + (z * z);
        return (sqlen < (1e-06 * 1e-06));

    }

    /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */
    inline vector3 getNormalise() const
    {
        return normalisedCopy();
    }

    /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */
    inline vector3 normalisedCopy(void) const
    {
        vector3 ret = *this;
        ret.normalise();
        return ret;
    }

    /** Calculates a reflection vector to the plane with the given normal .
    @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
    */
    inline vector3 reflect(const vector3& normal) const
    {
        return vector3( *this - ( 2 * this->dotProduct(normal) * normal ) );
    }

    /** Returns whether this vector is within a positional tolerance
        of another vector, also take scale of the vectors into account.
    @param rhs The vector to compare with
    @param tolerance The amount (related to the scale of vectors) that distance
        of the vector may vary by and still be considered close
    */
    inline bool positionCloses(const vector3& rhs, real tolerance = 1e-03f) const
    {
        return squaredDistance(rhs) <=
            (squaredLength() + rhs.squaredLength()) * tolerance;
    }

    /// Extract the primary (dominant) axis from this direction vector
    inline vector3 primaryAxis() const
    {
        real absx = math::abs(x);
        real absy = math::abs(y);
        real absz = math::abs(z);
        if (absx > absy)
            if (absx > absz)
                return x > 0 ? vector3::UNIT_X : vector3::NEGATIVE_UNIT_X;
            else
                return z > 0 ? vector3::UNIT_Z : vector3::NEGATIVE_UNIT_Z;
        else // absx <= absy
            if (absy > absz)
                return y > 0 ? vector3::UNIT_Y : vector3::NEGATIVE_UNIT_Y;
            else
                return z > 0 ? vector3::UNIT_Z : vector3::NEGATIVE_UNIT_Z;


    }

public:
    real x, y, z;

    static const vector3 ZERO;
    static const vector3 UNIT_X;
    static const vector3 UNIT_Y;
    static const vector3 UNIT_Z;
    static const vector3 NEGATIVE_UNIT_X;
    static const vector3 NEGATIVE_UNIT_Y;
    static const vector3 NEGATIVE_UNIT_Z;
    static const vector3 UNIT_SCALE;

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(z);
    }
};


std::ostream& operator<< (std::ostream& ostr, const vector3& toCast);
std::size_t hash_value(const vector3& value);


}
BLUB_CLASSVERSION(blub::vector3, 1)


#endif // BLUB_MATH_VECTOR3_HPP
