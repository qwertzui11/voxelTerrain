#ifndef VOXEL_TILE_BASE_HPP
#define VOXEL_TILE_BASE_HPP


#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/ownsOwnSharedPointer.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


template <typename classType>
class base : public enableSharedFromThis<classType>, public noncopyable
{
protected:
    typedef sharedPointer<classType> pointer;

    base()
    {

    }
};


}
}
}
}



#endif // VOXEL_TILE_BASE_HPP
