#ifndef PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP
#define PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP


#include "blub/core/globals.hpp"
#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/signal.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"

#include <boost/signals2/connection.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


class surface : public base<sharedPointer<tile::surface> >
{
public:
    typedef sharedPointer<tile::surface> t_tilePtr;
    typedef vector3int32 t_tileId;

    typedef base<t_tilePtr> t_base;

    typedef hashMap<t_tileId, t_tilePtr> t_tilesMap;
    typedef hashList<vector3int32> t_tileIdList;

    typedef sharedPointer<tile::accessor> t_tileAccessorPtr;
    typedef base<t_tileAccessorPtr> t_voxelAccessor;

    surface(blub::async::dispatcher &worker,
            t_voxelAccessor& voxels,
            const int32& lod,
            const real& voxelSize);
    virtual ~surface();

    int32 getTileCount() const;
    const real& getVoxelSize(void) const;

    t_tilePtr getTile(const blub::vector3int32& id) const;
    t_tilePtr getOrCreateTile(const blub::vector3int32& id) const;

    void editDone(void);

private:
    static t_tilePtr createTile(void);

    void setTileAccessorMaster(const accessor::t_tileId &id, const accessor::t_tilePtr &toSet);
    void removeTileMaster(const accessor::t_tileId& id);

    void editDoneMaster();

    void calculateSurfaceTS(const t_tileId id, accessor::t_tilePtr work, t_tilePtr workTile);
    void afterCalculateSurfaceMaster(const t_tileId& id, t_tilePtr workTile);


private:
    t_tilesMap m_tiles;

    t_voxelAccessor &m_voxels;
    int32 m_tileSize;
    int32 m_lod;
    int32 m_numTilesInWork;
    real m_voxelSize;

    boost::signals2::scoped_connection m_connTilesGotChanged;

};


}
}
}
}

#endif // PROCEDURAL_VOXEL_SIMPLE_SURFACE_HPP
