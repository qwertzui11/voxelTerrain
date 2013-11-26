#include "base.hpp"

#include "blub/math/vector3.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"


using namespace blub::procedural::voxel::edit;
using namespace blub::procedural::voxel;
using namespace blub;


base::base()
    : m_voxelContainer(nullptr)
    , m_cut(false)
{
}

base::~base()
{
}

void base::calculateVoxel(const transform &trans) const
{
    BASSERT(m_voxelContainer != nullptr);

    m_voxelContainer->editVoxel(t_base::getSharedThisPtr(), trans);
}

void base::calculateVoxel(tile::container* voxelContainer, const vector3int32 &voxelContainerOffset, const real &voxelScale, const blub::transform& trans) const
{
    const vector3int32 posContainerAbsolut(voxelContainerOffset*tile::container::voxelLength);
    for (int32 indX = 0; indX < tile::container::voxelLength; ++indX)
    {
        for (int32 indY = 0; indY < tile::container::voxelLength; ++indY)
        {
            for (int32 indZ = 0; indZ < tile::container::voxelLength; ++indZ)
            {
                const vector3int32 posVoxel(indX, indY, indZ);
                vector3 posAbsolut(posContainerAbsolut + posVoxel);

                posAbsolut -= trans.position;
                posAbsolut *= voxelScale;

                data voxelResult;
                const bool changeVoxel(calculateOneVoxel(posAbsolut, voxelScale, &voxelResult));

                if (!changeVoxel)
                {
                    continue;
                }
                if (!m_cut)
                {
                    voxelContainer->setVoxelIfInterpolationHigher(posVoxel, voxelResult);
                }
                else
                {
                    voxelResult.interpolation *= -1;
                    voxelContainer->setVoxelIfInterpolationLower(posVoxel, voxelResult);
                }
            }
        }
    }
}

void base::setCut(const bool &cut)
{
    m_cut = cut;
}

const bool &base::getCut() const
{
    return m_cut;
}

void base::setVoxelContainer(simple::container::base* toSet)
{
    m_voxelContainer = toSet;
}

simple::container::base* base::getVoxelContainer() const
{
    return m_voxelContainer;
}
