#ifndef MATH_HPP
#define MATH_HPP

#include "blub/core/globals.hpp"


namespace blub
{

class math
{
public:
    static const real pi;

    static real min(const real& num0, const real& num1);
    static int32 min(const int32& num0, const int32& num1);
    static real max(const real& num0, const real& num1);
    static int32 max(const int32& num0, const int32& num1);
    static real abs(const real& calc);
    static real saturate(const real& num, const real& min_, const real& max_);

    static real clamp(const real& val, const real& minval, const real& maxval);
    static int32 clamp(const int32& val, const int32& minval, const int32& maxval);

    static real floor(const real& calc);

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
