#include "renderer.hpp"

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


using namespace blub::procedural::voxel::terrain;
using namespace blub;



renderer::renderer(blub::async::dispatcher &worker,
                   t_rendererSurface &renderer_,
                   const t_createTileCallback &createTileCallback,
                   const t_syncRadiusList& syncRadien,
                   const real &voxelSize_)
    : m_worker(worker)
    , m_terrain(renderer_)
    , m_syncRadien(syncRadien)
{   
    BASSERT(m_terrain.getNumLod() <= (int32)m_syncRadien.size());

    real voxelSize(voxelSize_);

    for (uint32 indLod = 0; indLod < (uint32)m_terrain.getNumLod(); ++indLod)
    {
        if (indLod == 0)
        {
            m_lods.push_back(new t_simple(m_worker, m_terrain.getLod(indLod), createTileCallback, indLod, voxelSize, 0., syncRadien[0]));
        }
        else
        {
            m_lods.push_back(new t_simple(m_worker, m_terrain.getLod(indLod), createTileCallback, indLod, voxelSize, syncRadien[indLod-1], syncRadien[indLod]));
        }
        voxelSize*=2.;
    }
}

renderer::~renderer()
{
    for (auto lod : m_lods)
    {
        delete lod;
    }
    m_lods.clear();
}

void renderer::addCamera(t_cameraPtr toAdd, const vector3 &position)
{
    for (uint32 indLod = 0; indLod < m_lods.size(); ++indLod)
    {
        static_cast<simple::renderer*>(m_lods.at(indLod))->addCamera(toAdd, position);
    }
}

void renderer::updateCamera(t_cameraPtr toUpdate, const vector3 &position)
{
    for (uint32 indLod = 0; indLod < m_lods.size(); ++indLod)
    {
        static_cast<simple::renderer*>(m_lods.at(indLod))->updateCamera(toUpdate, position);
    }
}

void renderer::removeCamera(t_cameraPtr toRemove)
{
    for (uint32 indLod = 0; indLod < m_lods.size(); ++indLod)
    {
        static_cast<simple::renderer*>(m_lods.at(indLod))->removeCamera(toRemove);
    }
}

