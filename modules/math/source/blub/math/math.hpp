#ifndef MATH_HPP
#define MATH_HPP

#include "blub/core/globals.hpp"

#include <cmath>


namespace blub
{

class math
{
public:
    static constexpr real pi = 3.14159265358979323846;
    static constexpr real piHalf = math::pi/2.;
    static constexpr real sqrtZeroPointFive = 0.707106781;

    template <typename T>
    static T min(const T &val, const T &minimum)
    {
        return std::min(val, minimum);
    }
    template <typename T>
    static T max(const T &val, const T &maximum)
    {
        return std::max(val, maximum);
    }
    static real abs(const real& calc);

    template <typename T>
    static T clamp(const T& val, const T& minval, const T& maxval)
    {
        BASSERT(minval <= maxval);
        return std::max(std::min(val, maxval), minval);
    }

    static real map(const real &value, const real &fromMin, const real &fromMax, const real &toMin, const real &toMax);

    template <typename T>
    static T floor(const T& calc)
    {
        return std::floor(calc);
    }
    template <typename T>
    static T ceil(const T& val)
    {
        return std::ceil(val);
    }

    static real sin(const real& calc);
    static real asin(const real& calc);
    static real cos(const real& calc);
    static real acos(const real& calc);
    static real atan2(const real& fst, const real& snd);

    static real sqrt(const real& calc);

    static int32 pow(const int32& number, const int32& expo);
    static real pow(const real& number, const real& expo);

    static bool between(const real& number, const real& min, const real& max);

private:
    math()
    {;}

};

}

#endif // MATH_HPP
