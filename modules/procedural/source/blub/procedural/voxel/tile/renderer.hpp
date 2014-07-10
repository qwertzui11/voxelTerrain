#ifndef BLUB_PROCEDURAL_VOXEL_TILE_RENDERER_HPP
#define BLUB_PROCEDURAL_VOXEL_TILE_RENDERER_HPP

#include "blub/math/vector3int.hpp"
#include "blub/procedural/voxel/tile/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


/**
 * @brief The renderer class contains information if a tile::surface should get rendered and how it should get rendered.
 * Class must get derived by your render-engine. In the derived class convert the vertices/indices to your hardware/software render engine. (Most likely to a hardwarebuffer)
 */
template <class voxelType>
class renderer : public base<renderer<voxelType> >
{
public:
    typedef base<renderer<voxelType> > t_base;
    typedef blub::procedural::voxel::tile::surface<voxelType> t_tileData;
    typedef blub::sharedPointer<t_tileData> t_tileDataPtr;

    /**
     * @brief ~renderer destructor
     */
    virtual ~renderer()
    {
        ;
    }

    /**
     * @brief Implement this method and cast the data to your graphic engine.
     * @param convertToRenderAble Contains vertices and indices.
     * @param aabb The axisAlignedBox that describes the bound of the vertices.
     */
    virtual void setTileData(t_tileDataPtr convertToRenderAble, const axisAlignedBox &aabb) = 0;

    /**
     * @brief setVisible sets if a tile should get rendered. All lod-submeshes must not be rendered either.
     * @param vis true for visible.
     */
    virtual void setVisible(const bool& vis)
    {
        m_shouldBeVisible = vis;
    }

    /**
     * @brief setVisibleLod sets if one of the 6 crack closing submeshes (for lod) should get rendered.
     * @param indLod 0 to 6
     * @param vis
     */
    virtual void setVisibleLod(const uint16& indLod, const bool& vis)
    {
        m_lodShouldBeVisible[indLod] = vis;
    }

    /**
     * @brief getVisible returns if a tile should get rendered.
     * @return
     */
    const bool &getVisible() const
    {
        return m_shouldBeVisible;
    }

    /**
     * @brief getVisibleLod returns if a crack-closing lod-tile should get rendered.
     * @param indLod
     * @return
     */
    const bool &getVisibleLod(const uint16& indLod)
    {
        return m_lodShouldBeVisible[indLod];
    }

protected:
    /**
     * @brief renderer constructor.
     */
    renderer()
        : m_shouldBeVisible(false)
    {
        for (int32 ind = 0; ind < 6; ++ind)
        {
            m_lodShouldBeVisible[ind] = false;
        }
    }


protected:
    /**
     * @brief m_shouldBeVisible saves if tile should get rendered.
     */
    bool m_shouldBeVisible;
    /**
     * @brief saves if crack closing submesh should get rendered.
     */
    bool m_lodShouldBeVisible[6];


};


}
}
}
}

#endif // BLUB_PROCEDURAL_VOXEL_TILE_RENDERER_HPP
