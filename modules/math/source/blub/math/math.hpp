#ifndef BLUB_MATH_HPP
#define BLUB_MATH_HPP

#include "blub/core/globals.hpp"

#include <boost/config/suffix.hpp>

#include <algorithm>


namespace blub
{


class math
{
public:


#if defined(BOOST_NO_CXX11_CONSTEXPR)
    static const real math::pi;
    static const real math::piHalf;
    static const real math::sqrtZeroPointFive;
    static const real math::sqrtOneThird;
#else
    static constexpr real pi = 3.14159265358979323846;
    static constexpr real piHalf = pi / 2.;
    static constexpr real sqrtZeroPointFive = 0.707106781;
    static constexpr real sqrtOneThird = 0.577350269; // sqrt(1/3)
#endif


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
    template <typename T, typename pairType>
    static T clamp(const T& val, const pairType& minMaxVal)
    {
        BASSERT(minMaxVal.first <= minMaxVal.second);
        return clamp(val, minMaxVal.first, minMaxVal.second);
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

    template <typename T>
    static bool between(const T& number, const T& min, const T& max)
    {
        return (number >= min) && (number <= max);
    }

    static real sin(const real& calc);
    static real asin(const real& calc);
    static real cos(const real& calc);
    static real acos(const real& calc);
    static real atan2(const real& fst, const real& snd);

    static real sqrt(const real& calc);

    static int32 pow(const int32& number, const int32& expo);
    static real pow(const real& number, const real& expo);




private:
    math()
    {;}

};

}

#endif // BLUB_MATH_HPP
