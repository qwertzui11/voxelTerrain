#include "math.hpp"

#include <cmath>


using namespace blub;

const real math::pi = 3.14159265358979323846;


real math::min(const real &num0, const real &num1)
{
    return std::min<real>(num0, num1);
}

int32 math::min(const int32 &num0, const int32 &num1)
{
    return std::min<int32>(num0, num1);
}

real math::max(const real &num0, const real &num1)
{
    return std::max<real>(num0, num1);
}

int32 math::max(const int32 &num0, const int32 &num1)
{
    return std::max<int32>(num0, num1);
}

real math::abs(const real &calc)
{
    return std::abs(calc);
}

real math::saturate(const real &num, const real &min_, const real &max_)
{
    return min(max_, max(min_, num));
}

real math::clamp(const real &val, const real &minval, const real &maxval)
{
    BASSERT(minval <= maxval);
    return max(min(val, maxval), minval);
}

int32 math::clamp(const int32 &val, const int32 &minval, const int32 &maxval)
{
    BASSERT(minval <= maxval);
    return max(min(val, maxval), minval);
}

real math::floor(const real &calc)
{
    return std::floor(calc);
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
