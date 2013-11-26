#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


class renderer : public base<renderer>
{
public:
    typedef blub::procedural::voxel::tile::surface t_tileData;
    typedef blub::sharedPointer<tile::renderer> pointer;

    virtual ~renderer();

    virtual void setTileData(t_tileData *convertToRenderAble, const axisAlignedBox &aabb) = 0;

    virtual void setVisible(const bool& vis);
    virtual void setVisibleLod(const uint16& indLod, const bool& vis);
    const bool &getVisible(void) const;
    const bool &getVisibleLod(const uint16& indLod);

protected:
    renderer(const int32& lod, const vector3int32& id);

protected:
    const int32 m_lod;
    const vector3int32 m_id;

    bool m_shouldBeVisible;
    bool m_lodShouldBeVisible[6];


};


}
}
}
}

