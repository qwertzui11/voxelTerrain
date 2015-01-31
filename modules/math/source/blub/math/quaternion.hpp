#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "blub/core/globals.hpp"
#include "blub/math/predecl.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"

#ifdef BLUB_USE_PHYSX
#   include <foundation/PxQuat.h>
#endif


class btQuaternion;
namespace Ogre
{
    class Quaternion;
}


namespace blub
{


class quaternion
{
public:
    inline quaternion ()
        : w(1), x(0), y(0), z(0)
    {
    }
    /// Construct from an explicit list of values
    inline quaternion (
        real fX, real fY, real fZ, real fW)
        : w(fW), x(fX), y(fY), z(fZ)
    {
    }
    inline quaternion(const real& rfAngle, const vector3& rkAxis)
    {
        this->FromAngleAxis(rfAngle, rkAxis);
    }

#ifndef BLUB_NO_OGRE3D
    quaternion(const Ogre::Quaternion &quat);
    operator Ogre::Quaternion() const;
#endif
#ifndef BLUB_NO_BULLET
    quaternion(const btQuaternion &quat);
    operator btQuaternion() const;
#endif
#ifdef BLUB_USE_PHYSX
    quaternion(const physx::PxQuat& other)
        : w(other.w), x(other.x), y(other.y), z(other.z)
    {}
    operator physx::PxQuat() const
    {return physx::PxQuat(x,y,z,w);}
#endif

    /** Exchange the contents of this quaternion with another.
    */
    inline void swap(quaternion& other);

    /// Array accessor operator
    inline real operator [] ( const size_t i ) const
    {
        BASSERT( i < 4 );

        return *(&w+i);
    }

    /// Array accessor operator
    inline real& operator [] ( const size_t i )
    {
        BASSERT( i < 4 );

        return *(&w+i);
    }

    /// Pointer accessor for direct copying
    inline real* ptr()
    {
        return &w;
    }

    /// Pointer accessor for direct copying
    inline const real* ptr() const
    {
        return &w;
    }

    /*depricated inline quaternion& operator= (const quaternion& rkQ)
    {
        w = rkQ.w;
        x = rkQ.x;
        y = rkQ.y;
        z = rkQ.z;
        return *this;
    }*/
    quaternion operator+ (const quaternion& rkQ) const;
    quaternion operator- (const quaternion& rkQ) const;
    quaternion operator* (const quaternion& rkQ) const;
    quaternion operator* (real fScalar) const;
    friend quaternion operator* (real fScalar, const quaternion& rkQ)
    {
        return quaternion(fScalar*rkQ.x, fScalar*rkQ.y, fScalar*rkQ.z, fScalar*rkQ.w);
    }
    vector3 operator* (const vector3& rkVector) const;
    quaternion operator- () const;


    inline bool operator== (const quaternion& rhs) const
    {
        return (rhs.x == x) && (rhs.y == y) &&
            (rhs.z == z) && (rhs.w == w);
    }
    inline bool operator!= (const quaternion& rhs) const
    {
        return !operator==(rhs);
    }

    real Dot (const quaternion& rkQ) const;
    real Norm () const;
    real normalise();
    quaternion getNormalised() const;
    quaternion Inverse () const;  // apply to non-zero quaternion
    quaternion UnitInverse () const;  // apply to unit-length quaternion
    quaternion Exp () const;
    quaternion Log () const;
    void FromAngleAxis (const real& rfAngle, const vector3& rkAxis);

    static quaternion Slerp (real fT, const quaternion& rkP,
        const quaternion& rkQ, bool shortestPath = false);

    /** @see Slerp. It adds extra "spins" (i.e. rotates several times) specified
        by parameter 'iExtraSpins' while interpolating before arriving to the
        final values
    */
    static quaternion SlerpExtraSpins (real fT,
        const quaternion& rkP, const quaternion& rkQ,
        int iExtraSpins);

    // setup for spherical quadratic interpolation
    static void Intermediate (const quaternion& rkQ0,
        const quaternion& rkQ1, const quaternion& rkQ2,
        quaternion& rkA, quaternion& rkB);

    // spherical quadratic interpolation
    static quaternion Squad (real fT, const quaternion& rkP,
        const quaternion& rkA, const quaternion& rkB,
        const quaternion& rkQ, bool shortestPath = false);

    /** Performs Normalised linear interpolation between two quaternions, and returns the result.
        nlerp ( 0.0f, A, B ) = A
        nlerp ( 1.0f, A, B ) = B
        @remarks
        Nlerp is faster than Slerp.
        Nlerp has the proprieties of being commutative (@see Slerp;
        commutativity is desired in certain places, like IK animation), and
        being torque-minimal (unless shortestPath=false). However, it's performing
        the interpolation at non-constant velocity; sometimes this is desired,
        sometimes it is not. Having a non-constant velocity can produce a more
        natural rotation feeling without the need of tweaking the weights; however
        if your scene relies on the timing of the rotation or assumes it will point
        at a specific angle at a specific weight value, Slerp is a better choice.
    */
    static quaternion nlerp(real fT, const quaternion& rkP,
        const quaternion& rkQ, bool shortestPath = false);

    // special values
    static const quaternion ZERO;
    static const quaternion IDENTITY;

public:
    real w, x, y, z;

protected:
    BLUB_SERIALIZATION_ACCESS
    template<typename Archive>
    void serialize(Archive & readWrite, const unsigned int version)
    {
        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(x);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(y);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(z);
        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(w);
    }

};


std::ostream& operator << (std::ostream& ostr, const quaternion& toCast);


}

#endif // QUATERNION_HPP
