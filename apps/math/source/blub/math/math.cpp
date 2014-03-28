#include "math.hpp"


using namespace blub;

constexpr real math::pi;
constexpr real math::piHalf;


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

bool math::between(const real &number, const real &min, const real &max)
{
    return (number > min) && (number < max);
}
