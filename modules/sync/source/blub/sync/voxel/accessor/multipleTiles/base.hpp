#ifndef NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_BASE_HPP
#define NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_BASE_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/format/binary/input.hpp"
#include "blub/serialization/format/binary/output.hpp"


namespace blub
{
namespace sync
{
namespace voxel
{
namespace accessor
{
namespace multipleTiles
{


enum class sendType : uint8
{
    lockForEdit,
    unlockForEdit,
    setTile,
    removeTile
};




}
}
}
}
}



#endif // NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_BASE_HPP
