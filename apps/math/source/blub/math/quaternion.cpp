#include "blub/math/quaternion.hpp"
#include "blub/math/vector3.hpp"

#include <utility>

#ifndef BLUB_NO_BULLET
#   include <bullet/LinearMath/btQuaternion.h>
#endif
#ifndef BLUB_NO_OGRE3D
#   include <OGRE/OgreQuaternion.h>
#endif


using namespace blub;


const quaternion quaternion::ZERO = quaternion(0., 0., 0., 0.);
const quaternion quaternion::IDENTITY = quaternion(0., 0., 0., 1.);
const real msEpsilon = 1e-03;

#ifndef BLUB_NO_OGRE3D
quaternion::quaternion(const Ogre::Quaternion &quat)
    : w(quat.w)
    , x(quat.x)
    , y(quat.y)
    , z(quat.z)
{;}

quaternion::operator Ogre::Quaternion() const
{
    return Ogre::Quaternion(w, x, y, z);
}
#endif
#ifndef BLUB_NO_BULLET
quaternion::quaternion(const btQuaternion &quat)
    : w(quat.getW())
    , x(quat.getX())
    , y(quat.getY())
    , z(quat.getZ())
{;}

quaternion::operator btQuaternion() const
{
    return btQuaternion(x, y, z, w);
}
#endif


void quaternion::swap(blub::quaternion &other)
{
    std::swap(w, other.w);
    std::swap(x, other.x);
    std::swap(y, other.y);
    std::swap(z, other.z);
}

quaternion quaternion::operator +(const quaternion &rkQ) const
{
    return quaternion(x+rkQ.x, y+rkQ.y, z+rkQ.z, w+rkQ.w);
}

quaternion quaternion::operator -(const quaternion &rkQ) const
{
    return quaternion(x-rkQ.x, y-rkQ.y, z-rkQ.z, w-rkQ.w);
}

quaternion quaternion::operator *(const quaternion &rkQ) const
{
    return quaternion
    (
        w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
        w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
        w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x,
        w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z
    );
}

quaternion quaternion::operator *(real fScalar) const
{
    return quaternion(fScalar*x, fScalar*y, fScalar*z, fScalar*w);
}

vector3 quaternion::operator *(const vector3 &v) const
{
    // nVidia SDK implementation
    vector3 uv, uuv;
    vector3 qvec(x, y, z);
    uv = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}

quaternion quaternion::operator -() const
{
    return quaternion(-x, -y, -z, -w);
}

real quaternion::Dot(const quaternion &rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}

real quaternion::Norm() const
{
    return w*w+x*x+y*y+z*z;
}

real quaternion::normalise()
{
    real len = Norm();
    real factor = 1.0f / math::sqrt(len);
    *this = *this * factor;
    return len;
}

quaternion quaternion::getNormalised() const
{
    quaternion result(*this);
    result.normalise();
    return result;
}

quaternion quaternion::Inverse() const
{
    real fNorm = w*w+x*x+y*y+z*z;
    if ( fNorm > 0.0 )
    {
        real fInvNorm = 1.0f/fNorm;
        return quaternion(-x*fInvNorm, -y*fInvNorm, -z*fInvNorm, w*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

quaternion quaternion::UnitInverse() const
{
    return quaternion(-x, -y, -z, w);
}

quaternion quaternion::Exp() const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    real fAngle ( math::sqrt(x*x+y*y+z*z) );
    real fSin = math::sin(fAngle);

    quaternion kResult;
    kResult.w = math::cos(fAngle);

    if ( math::abs(fSin) >= msEpsilon )
    {
        real fCoeff = fSin/fAngle;
        kResult.x = fCoeff*x;
        kResult.y = fCoeff*y;
        kResult.z = fCoeff*z;
    }
    else
    {
        kResult.x = x;
        kResult.y = y;
        kResult.z = z;
    }

    return kResult;
}

quaternion quaternion::Log() const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    quaternion kResult;
    kResult.w = 0.0;

    if (math::abs(w) < 1.0 )
    {
        real fAngle ( math::acos(w) );
        real fSin = math::sin(fAngle);
        if (math::abs(fSin) >= msEpsilon )
        {
            real fCoeff = fAngle/fSin;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
            return kResult;
        }
    }

    kResult.x = x;
    kResult.y = y;
    kResult.z = z;

    return kResult;
}

void quaternion::FromAngleAxis(const real &rfAngle, const vector3 &rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    real fHalfAngle ( 0.5*rfAngle );
    real fSin = math::sin(fHalfAngle);
    w = math::cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}


quaternion quaternion::Slerp(real fT, const quaternion &rkP, const quaternion &rkQ, bool shortestPath)
{
    real fCos = rkP.Dot(rkQ);
    quaternion rkT;

    // Do we need to invert rotation?
    if (fCos < 0.0f && shortestPath)
    {
        fCos = -fCos;
        rkT = -rkQ;
    }
    else
    {
        rkT = rkQ;
    }

    if (math::abs(fCos) < 1 - msEpsilon)
    {
        // Standard case (slerp)
        real fSin = math::sqrt(1 - fCos*fCos);
        real fAngle = math::atan2(fSin, fCos);
        real fInvSin = 1.0f / fSin;
        real fCoeff0 = math::sin((1.0f - fT) * fAngle) * fInvSin;
        real fCoeff1 = math::sin(fT * fAngle) * fInvSin;
        return fCoeff0 * rkP + fCoeff1 * rkT;
    }
    else
    {
        // There are two situations:
        // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        quaternion t = (1.0f - fT) * rkP + fT * rkT;
        // taking the complement requires renormalisation
        t.normalise();
        return t;
    }
}

quaternion quaternion::SlerpExtraSpins(real fT, const quaternion& rkP, const quaternion& rkQ, int iExtraSpins)
{
    real fCos = rkP.Dot(rkQ);
    real fAngle (math::acos(fCos) );

    if (math::abs(fAngle) < msEpsilon )
       return rkP;

    real fSin = math::sin(fAngle);
    real fPhase ( math::pi*iExtraSpins*fT );
    real fInvSin = 1.0f/fSin;
    real fCoeff0 = math::sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
    real fCoeff1 = math::sin(fT*fAngle + fPhase)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}

void quaternion::Intermediate(const quaternion &rkQ0, const quaternion &rkQ1, const quaternion &rkQ2, quaternion &rkA, quaternion &rkB)
{
    // assert:  q0, q1, q2 are unit quaternions

    quaternion kQ0inv = rkQ0.UnitInverse();
    quaternion kQ1inv = rkQ1.UnitInverse();
    quaternion rkP0 = kQ0inv*rkQ1;
    quaternion rkP1 = kQ1inv*rkQ2;
    quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
    quaternion kMinusArg = -kArg;

    rkA = rkQ1*kArg.Exp();
    rkB = rkQ1*kMinusArg.Exp();
}

quaternion quaternion::Squad(real fT, const quaternion &rkP, const quaternion &rkA, const quaternion &rkB, const quaternion &rkQ, bool shortestPath)
{
    real fSlerpT = 2.0f*fT*(1.0f-fT);
    quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
    quaternion kSlerpQ = Slerp(fT, rkA, rkB);
    return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
}

quaternion quaternion::nlerp(real fT, const quaternion &rkP, const quaternion &rkQ, bool shortestPath)
{
    quaternion result;
    real fCos = rkP.Dot(rkQ);
    if (fCos < 0.0f && shortestPath)
    {
        result = rkP + fT * ((-rkQ) - rkP);
    }
    else
    {
        result = rkP + fT * (rkQ - rkP);
    }
    result.normalise();
    return result;
}

std::ostream &blub::operator<< (std::ostream &ostr, const quaternion &toCast)
{
    return ostr << "(" << toCast.x << "," << toCast.y << "," << toCast.z << "," << toCast.w << ")";
}
