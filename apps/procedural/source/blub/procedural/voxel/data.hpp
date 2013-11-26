#ifndef VOXEL_DATA_HPP
#define VOXEL_DATA_HPP

#include "blub/core/classVersion.hpp"
#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{


class data
{
public:
    data()
        : interpolation(-127)
    {
        ;
    }
    data(const int8& interpolation_)
        : interpolation(interpolation_)
    {
        ;
    }
    bool operator == (const data& other) const
    {
        return other.interpolation == interpolation;
    }
    bool operator != (const data& other) const
    {
        return other.interpolation != interpolation;
    }


private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        (void)version;

        readWrite & BLUB_SERIALIZATION_NAMEVALUEPAIR(interpolation);
    }


public:
    int8 interpolation;


};


}
}
}
BLUB_CLASSVERSION(blub::procedural::voxel::data, 1)


#endif // VOXEL_DATA_HPP
