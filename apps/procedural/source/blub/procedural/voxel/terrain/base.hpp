#ifndef PROCEDURAL_VOXEL_TERRAIN_BASE_HPP
#define PROCEDURAL_VOXEL_TERRAIN_BASE_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{


template <class tileType>
class base : public noncopyable
{
public:
    typedef tileType t_tile;
    typedef procedural::voxel::simple::base<t_tile>* t_lod;
    typedef vector<t_lod> t_lodList;

    base();
    virtual ~base();

    t_lod getLod(const uint16& lod) const;
    const t_lodList& getLodList(void) const;

    int32 getNumLod(void) const;

protected:


protected:
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
const typename base<tileType>::t_lodList& base<tileType>::getLodList(void) const
{
    return m_lods;
}

template <class tileType>
int32 base<tileType>::getNumLod() const
{
    return m_lods.size();
}


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_BASE_HPP
