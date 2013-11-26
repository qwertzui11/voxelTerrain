#include "renderer.hpp"

using namespace blub::procedural::voxel::tile;
using namespace blub;



renderer::~renderer()
{
    ;
}

void renderer::setVisible(const bool &vis)
{
    m_shouldBeVisible = vis;
}

void renderer::setVisibleLod(const uint16 &indLod, const bool &vis)
{
    m_lodShouldBeVisible[indLod] = vis;
}

const bool &renderer::getVisible() const
{
    return m_shouldBeVisible;
}


const bool &renderer::getVisibleLod(const blub::uint16 &indLod)
{
    return m_lodShouldBeVisible[indLod];
}

renderer::renderer(const int32 &lod, const vector3int32 &id)
    : m_lod(lod)
    , m_id(id)
    , m_shouldBeVisible(false)
{
    for (int32 ind = 0; ind < 6; ++ind)
    {
        m_lodShouldBeVisible[ind] = false;
    }
}
