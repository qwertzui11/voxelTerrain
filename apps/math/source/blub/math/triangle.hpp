#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "blub/core/list.hpp"
#include "blub/core/globals.hpp"

namespace blub
{

class triangle
{
public:
    triangle()
    {;}
    triangle(blub::uint32 a, blub::uint32 b, blub::uint32 c)
    {
        ind[0] = a;
        ind[1] = b;
        ind[2] = c;
    }

    bool contains(blub::uint32 _ind)
    {
        return ind[0] == _ind || ind[1] == _ind || ind[2] == _ind;
    }

    bool valid()
    {
        return ind[0] != ind[1] && ind[0] != ind[2] && ind[1] != ind[2];
    }

    bool operator == (const triangle & other) const
    {
        list<uint32> otherVec;
        for (uint16 ind1 = 0; ind1 < 3; ++ind1)
            otherVec.push_back(other.ind[ind1]);
        for (uint16 ind1 = 0; ind1 < 3; ++ind1)
        {
            int32 index = otherVec.indexOf(ind[ind1]);
            if (index == -1)
                return false;
            else
                otherVec.removeAt(index);
        }
        return true;
    }
    triangle operator + (const int32 & other) const
    {
        return triangle(ind[0]+other, ind[1]+other, ind[2]+other);
    }

    blub::uint16 ind[3];
};

}

#endif // TRIANGLE_HPP
