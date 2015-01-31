#include "math.hpp"

#include <cstdlib>


using namespace blub;

#if defined(BOOST_NO_CXX11_CONSTEXPR)
const real math::pi = 3.14159265358979323846;
const real math::piHalf = math::pi / 2.;
const real math::sqrtZeroPointFive = 0.707106781;
const real math::sqrtOneThird = 0.577350269;
#else
constexpr real math::pi;
constexpr real math::piHalf;
constexpr real math::sqrtZeroPointFive;
constexpr real math::sqrtOneThird;
#endif


real math::abs(const real &calc)
{
    return std::abs(calc);
}

real math::map(const real &value, const real &fromMin, const real &fromMax, const real &toMin, const real &toMax)
{
    BASSERT(between(value, fromMin, fromMax));
    BASSERT(toMin < toMax);
    return clamp(((value-fromMin)/(fromMax-fromMin))*(toMax-toMin)+toMin, toMin, toMax);
}

real math::sin(const real &calc)
{
    return std::sin(calc);
}

real math::asin(const real &calc)
{
    return std::asin(calc);
}

real math::cos(const real &calc)
{
    return std::cos(calc);
}

real math::acos(const real &calc)
{
    return std::acos(calc);
}

real math::atan2(const real &fst, const real &snd)
{
    return std::atan2(fst, snd);
}

real math::sqrt(const real &calc)
{
    return std::sqrt(calc);
}

int32 math::pow(const int32 &number, const int32 &expo)
{
    return (int32)std::pow((real)number, (int)expo);
}

real math::pow(const real &number, const real &expo)
{
    return std::pow(number, expo);
}

uint32 math::rand()
{
    return std::rand();
}

real math::randReal()
{
    return real(rand()) / real(RAND_MAX);
}

