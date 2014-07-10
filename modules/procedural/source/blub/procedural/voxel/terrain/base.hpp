#ifndef PROCEDURAL_VOXEL_TERRAIN_BASE_HPP
#define PROCEDURAL_VOXEL_TERRAIN_BASE_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/predecl.hpp"
#include "blub/procedural/voxel/simple/base.hpp"

#include <boost/function/function_fwd.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{


/**
 * @brief The base class gets derived by every class in the namesapce terrain.
 * It holds various level of details, always of type procedural::voxel::simple::base.
 */
template <class tileType>
class base : public noncopyable
{
public:
    typedef tileType t_tile;
    typedef procedural::voxel::simple::base<t_tile>* t_lod;
    typedef vector<t_lod> t_lodList;

    /**
     * @brief base contructor
     */
    base();
    /**
     * @brief ~base destructor
     */
    virtual ~base();

    /**
     * @brief getLod returns a level of detail.
     * @param lod Lod-index starting with zero.
     * @return
     */
    t_lod getLod(const uint16& lod) const;
    /**
     * @brief getLodList returns all level of details.
     * @return
     */
    const t_lodList& getLodList() const;

    /**
     * @brief getNumLod returns number of level of details.
     * @return Never negative.
     */
    int32 getNumLod() const;

    typedef typename simple::base<t_tile>::t_createTileCallback t_createTileCallback;
    /**
     * @brief setCreateTileCallback sets the callback for creating tiles to the lods.
     * @param toSet
     */
    void setCreateTileCallback(const t_createTileCallback &toSet);

protected:


protected:
    /**
     * @brief m_lods container for the lods.
     */
    t_lodList m_lods;

};

template <class tileType>
base<tileType>::base()
{
    ;
}
template <class tileType>
base<tileType>::~base()
{
    ;
}

template <class tileType>
typename base<tileType>::t_lod base<tileType>::getLod(const uint16 &lod) const
{
    BASSERT(lod < m_lods.size());
    return m_lods[lod];
}

template <class tileType>
const typename base<tileType>::t_lodList& base<tileType>::getLodList() const
{
    return m_lods;
}

template <class tileType>
int32 base<tileType>::getNumLod() const
{
    return m_lods.size();
}

template <class tileType>
void base<tileType>::setCreateTileCallback(const base::t_createTileCallback &toSet)
{
    for (t_lod &lod : m_lods)
    {
        lod->setCreateTileCallback(toSet);
    }
}


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_BASE_HPP