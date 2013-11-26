#include "axisAlignedBox.hpp"

#include "blub/math/math.hpp"
#include "blub/math/transform.hpp"
#include "blub/procedural/voxel/data.hpp"


using namespace blub::procedural::voxel::edit;
using namespace blub::procedural::voxel;


axisAlignedBox::axisAlignedBox(const blub::axisAlignedBox &desc)
    : m_aab(desc)
{
    ;
}

axisAlignedBox::pointer axisAlignedBox::create(const blub::axisAlignedBox &desc)
{
    return pointer(new axisAlignedBox(desc));
}

axisAlignedBox::~axisAlignedBox()
{
    ;
}

void axisAlignedBox::setAxisAlignedBox(const blub::axisAlignedBox &desc)
{
    m_aab = desc;
}

const blub::axisAlignedBox &axisAlignedBox::getAxisAlignedBox() const
{
    return m_aab;
}

blub::axisAlignedBox axisAlignedBox::getAxisAlignedBoundingBox(const blub::real &voxelSize, const transform &trans) const
{
    return blub::axisAlignedBox(m_aab.getMinimum() / voxelSize + trans.position,
                                m_aab.getMaximum() / voxelSize + trans.position);
}

bool axisAlignedBox::calculateOneVoxel(const blub::vector3 &pos, const blub::real &voxelSize, data *resultVoxel) const
{
    (void)voxelSize;

    if (m_aab.contains(pos))
    {
        resultVoxel->interpolation = 127;
        return true;
    }
    else
    {
        //resultVoxel->interpolation = -127;
        return false;
    }
}
