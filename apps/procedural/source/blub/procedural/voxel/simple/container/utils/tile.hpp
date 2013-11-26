#ifndef TILEHOLDER_HPP
#define TILEHOLDER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{
namespace container
{
namespace utils
{


enum class tileState
{
    full,
    empty,
    partitial
};
class tile
{
public:
    typedef sharedPointer<voxel::tile::container> t_tilePtr;

    tile()
        : state(tileState::empty)
    {
        ;
    }
    tile(const tileState& state_)
        : state(state_)
    {
        ;
    }

    tileState state;
    t_tilePtr data;
};


}
}
}
}
}
}


#endif // TILEHOLDER_HPP
