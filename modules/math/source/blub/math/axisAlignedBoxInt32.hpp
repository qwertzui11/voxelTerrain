#ifndef AXISALIGNEDBOXINT32_HPP
#define AXISALIGNEDBOXINT32_HPP

#include "blub/math/axisAlignedBoxTemplate.hpp"
#include "blub/math/vector3int.hpp"


namespace blub
{


class axisAlignedBoxInt32 : public axisAlignedBoxTemplate<vector3int32>
{
public:
    typedef axisAlignedBoxTemplate<vector3int32> t_base;

    axisAlignedBoxInt32()
    {
        setInvalid();
    }

    axisAlignedBoxInt32(const vector3int32& min, const vector3int32& max)
        : t_base(min, max)
    {
        ;
    }

    void setInvalid(void)
    {
        setMinimumAndMaximum(vector3int32(1), vector3int32(0));
    }

};


}


#endif // AXISALIGNEDBOXINT32_HPP
