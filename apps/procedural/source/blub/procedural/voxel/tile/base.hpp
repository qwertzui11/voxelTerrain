#ifndef BLUB_PROCEDURAL_VOXEL_TILE_BASE_HPP
#define BLUB_PROCEDURAL_VOXEL_TILE_BASE_HPP


#include "blub/core/enableSharedFromThis.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


/**
 * @brief The base class gets derived by every class in the namespace tile::*.
 * Never instance this class and derived classes without a shared pointer.
 */
template <typename classType>
class base : public enableSharedFromThis<classType>, public noncopyable
{
public:
    typedef sharedPointer<classType> pointer;

protected:
    /**
     * @brief base constrcutor.
     */
    base()
    {

    }
};


}
}
}
}



#endif // BLUB_PROCEDURAL_VOXEL_TILE_BASE_HPP
