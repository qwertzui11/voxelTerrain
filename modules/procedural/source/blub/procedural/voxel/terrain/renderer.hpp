#ifndef BLUB_PROCEDURAL_VOXEL_TERRAIN_RENDERER_HPP
#define BLUB_PROCEDURAL_VOXEL_TERRAIN_RENDERER_HPP

#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"

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
template <class configType>
class renderer : public base<typename configType::t_renderer::t_simple>
{
public:
    typedef configType t_config;
    typedef typename t_config::t_renderer::t_simple t_simple;
    typedef base<t_simple> t_base;
    typedef sharedPointer<sync::identifier> t_cameraPtr;

    typedef vector<real> t_syncRadiusList;

    typedef typename t_config::t_surface::t_terrain t_rendererSurface;


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
                t_base::m_lods.emplace_back(new t_simple(m_worker, m_terrain.getLod(indLod), indLod, 0., syncRadien[0]));
            }
            else
            {
                t_base::m_lods.emplace_back(new t_simple(m_worker, m_terrain.getLod(indLod), indLod, syncRadien[indLod-1], syncRadien[indLod]));
            }
        }
    }

    /**
     * @brief ~renderer destructor
     */
    ~renderer()
    {
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
            t_base::m_lods[indLod]->addCamera(toAdd, position);
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
            t_base::m_lods[indLod]->updateCamera(toUpdate, position);
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
            t_base::m_lods[indLod]->removeCamera(toRemove);
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
