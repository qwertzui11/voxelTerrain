#ifndef BLUB_PROCEDURAL_VOXEL_TERRAIN_RENDERER_HPP
#define BLUB_PROCEDURAL_VOXEL_TERRAIN_RENDERER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"

#include "blub/core/hashMap.hpp"
#include "blub/core/log.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/math/octree/search.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/simple/renderer.hpp"
#include "blub/procedural/voxel/simple/surface.hpp"
#include "blub/procedural/voxel/terrain/surface.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"
#include "blub/sync/sender.hpp"

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
 * @brief The renderer class contains a custom count of simple::renderer for level of detail.
 */
template <class voxelType>
class renderer : public base<sharedPointer<tile::renderer<voxelType> > >
{
public:
    typedef tile::renderer<voxelType> t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;
    typedef base<t_tilePtr> t_base;
    typedef sharedPointer<sync::identifier> t_cameraPtr;

    typedef sharedPointer<procedural::voxel::tile::surface<voxelType> > t_tileDataPtr;
    typedef vector<real> t_syncRadiusList;

    typedef simple::renderer<voxelType> t_simple;
    typedef base<t_tileDataPtr> t_rendererSurface;


    /**
     * @brief renderer constructor
     * @param worker May get called by several threads.
     * @param renderer_ The surface to sync with.
     * @param syncRadien The radien of which of every lod should get rendered.
     */
    renderer(blub::async::dispatcher &worker,
             t_rendererSurface &renderer_,
             const t_syncRadiusList& syncRadien)
        : m_worker(worker)
        , m_terrain(renderer_)
        , m_syncRadien(syncRadien)
    {
        BASSERT(m_terrain.getNumLod() <= (int32)m_syncRadien.size());

        for (uint32 indLod = 0; indLod < (uint32)m_terrain.getNumLod(); ++indLod)
        {
            if (indLod == 0)
            {
                t_base::m_lods.push_back(new t_simple(m_worker, m_terrain.getLod(indLod), indLod, 0., syncRadien[0]));
            }
            else
            {
                t_base::m_lods.push_back(new t_simple(m_worker, m_terrain.getLod(indLod), indLod, syncRadien[indLod-1], syncRadien[indLod]));
            }
        }
    }

    /**
     * @brief ~renderer destructor
     */
    virtual ~renderer()
    {
        for (auto lod : t_base::m_lods)
        {
            delete lod;
        }
        t_base::m_lods.clear();
    }

    /**
     * @brief addCamera adds an camera.
     * @param toAdd Must not be nullptr
     * @param position The initial position of the camera.
     */
    void addCamera(t_cameraPtr toAdd, const blub::vector3& position)
    {
        for (uint32 indLod = 0; indLod < t_base::m_lods.size(); ++indLod)
        {
            static_cast<t_simple*>(t_base::m_lods.at(indLod))->addCamera(toAdd, position);
        }
    }
    /**
     * @brief updateCamera updates the position of a camera you have to add before by using addCamera()
     * @param toUpdate The camera, must not be nullptr.
     * @param position The new position.
     */
    void updateCamera(t_cameraPtr toUpdate, const blub::vector3& position)
    {
        for (uint32 indLod = 0; indLod < t_base::m_lods.size(); ++indLod)
        {
            static_cast<t_simple*>(t_base::m_lods.at(indLod))->updateCamera(toUpdate, position);
        }
    }
    /**
     * @brief removeCamera removes a camera.
     * @param toRemove Must not be nullptr.
     */
    void removeCamera(t_cameraPtr toRemove)
    {
        for (uint32 indLod = 0; indLod < t_base::m_lods.size(); ++indLod)
        {
            static_cast<t_simple*>(t_base::m_lods.at(indLod))->removeCamera(toRemove);
        }
    }

protected:


private:
    blub::async::dispatcher &m_worker;
    t_rendererSurface &m_terrain;

    t_syncRadiusList m_syncRadien;

};



}
}
}
}

#endif // BLUB_PROCEDURAL_VOXEL_TERRAIN_RENDERER_HPP
