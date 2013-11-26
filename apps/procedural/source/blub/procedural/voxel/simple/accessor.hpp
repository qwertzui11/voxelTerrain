#ifndef PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP

#include "blub/core/globals.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/async/mutexReadWrite.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/signal.hpp"
#include "blub/procedural/voxel/simple/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


class accessor : public base<sharedPointer<tile::accessor> >, public noncopyable
{
public:
    typedef vector3int32 t_tileId;
    typedef sharedPointer<tile::accessor> t_tilePtr;
    typedef hashMap<vector3int32, t_tilePtr> t_tiles;

    typedef base<t_tilePtr> t_base;

    typedef sharedPointer<tile::container> t_tileContainerPtr;
    typedef hashList<vector3int32> t_tileIdList;
    typedef container::utils::tileState t_tileState;
    typedef container::utils::tile t_tileHolder;


    accessor(async::dispatcher &worker,
             container::base &voxels,
             const int32& lod);
    virtual ~accessor();

    void tilesGotChanged();

    container::base &getVoxelContainer(void) const;

    t_tilePtr getTile(const t_tileId& id) const;

private:
    void tilesGotChangedMaster();

    void calculateAccessorTS(const t_tileId& id, t_tilePtr workTile);
    void afterCalculateAccessorMaster(const t_tileId& id, t_tilePtr workTile, const bool &didValuesChanged);

    void calculateAffectedAccessorTilesByContainerTile(const t_tileId& conterainerId, const t_tileHolder &holder, t_tileIdList& resultingSurfaceTiles);

    t_tilePtr createTile(void) const;

private:
    data getVoxelData(const vector3int32& pos, axisAlignedBoxInt32& lastUsedTileBounds, t_tileHolder& lastUsedTile);

    container::base& m_voxels;
    const int32 m_lod;
    const int32 m_voxelSkip;
    int32 m_numTilesInWork;

    t_tiles m_tiles;

    t_sigEditDone m_sigEditDone;

    boost::signals2::scoped_connection m_connTilesGotChanged;
};


}
}
}
}


#endif // PROCEDURAL_VOXEL_ACCESSOR_ACCESSOR_HPP
