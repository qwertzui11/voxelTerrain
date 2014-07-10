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


/**
 * @brief The tileState enum defines opitimized states for a tile.
 */
enum class tileState
{
    full,
    empty,
    partitial
};


/**
 * @brief The tile class contains a state and the data itself. For memory optimization no data gets saved if state is full or empty
 */
template <class voxelType>
class tile
{
public:
    typedef sharedPointer<voxel::tile::container<voxelType> > t_tilePtr;

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
