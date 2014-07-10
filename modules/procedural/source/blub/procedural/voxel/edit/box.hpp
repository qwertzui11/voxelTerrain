#ifndef PROCEDURAL_VOXEL_EDIT_BOX_HPP
#define PROCEDURAL_VOXEL_EDIT_BOX_HPP

#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/plane.hpp"
#include "blub/math/ray.hpp"
#include "blub/procedural/voxel/edit/base.hpp"

#include "blub/core/sharedPointer.hpp"
#include "blub/math/intersection.hpp"
#include "blub/math/ray.hpp"
#include "blub/math/transform.hpp"
#include "blub/procedural/voxel/tile/container.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


/**
 * @brief a voxel-generator defined by a transform-able-box.
 */
template <class voxelType>
class box : public base<voxelType>
{
public:
    typedef base<voxelType> t_base;
    typedef sharedPointer<box<voxelType> > pointer;

    /**
     * @brief create creates an instance of box.
     * @param size The scale of the box.
     * @param rotation The orientation of the box.
     * @return Returns an initialized shared_ptr of box
     */
    static pointer create(const vector3 &size, const quaternion &rotation)
    {
        return pointer(new box(size, rotation));
    }

    /**
     * @brief getAxisAlignedBoundingBox returns the scaled, transformed bounding box of the to calculate voxels
     * @param trans The transfrom
     */
    blub::axisAlignedBox getAxisAlignedBoundingBox(const transform &trans) const override
    {
        return blub::axisAlignedBox(m_aabb.getMinimum()*trans.scale + trans.position,
                                    m_aabb.getMaximum()*trans.scale + trans.position);
    }

protected:
    /**
     * @brief constructor. Sets up the cutPlanes of the box
     * @param size The Scale of the box.
     * @param rotation The rotatation of the box.
     */
    box(const vector3 &size, const quaternion &rotation)
        : m_size(size)
        , m_rotation(rotation.getNormalised())
        , m_aabb(-size.length(), size.length())
    {
        BASSERT(size.x > 0.);
        BASSERT(size.y > 0.);
        BASSERT(size.z > 0.);

        {
            const blub::vector3 rotatedUp(m_rotation*vector3(0., m_size.y, 0.));
            const blub::vector3 normalUp(rotatedUp.getNormalise());
            m_planes[0] = blub::plane(rotatedUp, normalUp);
            m_planes[1] = blub::plane(-rotatedUp, -normalUp);
        }
        {
            const blub::vector3 rotatedRight(m_rotation*vector3(m_size.x, 0., 0.));
            const blub::vector3 normalRight(rotatedRight.getNormalise());
            m_planes[2] = blub::plane(rotatedRight, normalRight);
            m_planes[3] = blub::plane(-rotatedRight, -normalRight);
        }
        {
            const blub::vector3 rotatedFront(m_rotation*vector3(0., 0., m_size.z));
            const blub::vector3 normalFront(rotatedFront.getNormalise());
            m_planes[4] = blub::plane(rotatedFront, normalFront);
            m_planes[5] = blub::plane(-rotatedFront, -normalFront);
        }
    }


    /**
     * @brief calculateVoxel cuts the box in voxelLines.
     * Only sets values if voxel is inside box.
     * @param voxelContainer Must not be nullptr
     * @param voxelContainerOffset the absolute position of the container
     * @param trans The transform
     */
    void calculateVoxel(typename t_base::t_voxelContainerTile *voxelContainer, const vector3int32 &voxelContainerOffset, const transform &trans) const override
    {
        const vector3int32 posContainerAbsolut(voxelContainerOffset*t_base::t_voxelContainerTile::voxelLength);

        vector3int32 toLoop[] = {{1, t_base::t_voxelContainerTile::voxelLength, t_base::t_voxelContainerTile::voxelLength},
                                 {t_base::t_voxelContainerTile::voxelLength, 1, t_base::t_voxelContainerTile::voxelLength},
                                 {t_base::t_voxelContainerTile::voxelLength, t_base::t_voxelContainerTile::voxelLength, 1}
                                };
        vector3 rayDir[] = {{1., 0., 0.},
                            {0., 1., 0.},
                            {0., 0., 1.}};
        for (int32 indAxis = 0; indAxis < 3; ++indAxis)
        {
            for (int32 indX = 0; indX < toLoop[indAxis].x; ++indX)
            {
                for (int32 indY = 0; indY < toLoop[indAxis].y; ++indY)
                {
                    for (int32 indZ = 0; indZ < toLoop[indAxis].z; ++indZ)
                    {
                        const vector3int32 posVoxel(indX, indY, indZ);
                        vector3 posAbsolut(posContainerAbsolut + posVoxel);

                        posAbsolut -= trans.position;
                        posAbsolut /= trans.scale;

                        const ray test(posAbsolut, rayDir[indAxis]);
                        vector3 cutPoint;

                        real cutPoints[2];
                        blub::plane cutPlanes[2];
                        blub::uint16 numCutPointsFound(0);

                        for (blub::int32 indPlane = 0; indPlane < 6; ++indPlane)
                        {
                            if (test.intersects(m_planes[indPlane], &cutPoint))
                            {
                                bool pointOnBoxSurface(true);
                                for (blub::int32 indPlane2 = 0; indPlane2 < 6; ++indPlane2)
                                {
                                    if (indPlane2 == indPlane)
                                    {
                                        continue;
                                    }
                                    if (m_planes[indPlane2].getDistance(cutPoint) > 0.0)
                                    {
                                        pointOnBoxSurface = false;
                                        break;
                                    }
                                }
                                if (!pointOnBoxSurface)
                                {
                                    continue;
                                }
                                cutPoints[numCutPointsFound] = (cutPoint[indAxis]*trans.scale[indAxis] + trans.position[indAxis]) - static_cast<real>(posContainerAbsolut[indAxis]);
                                cutPlanes[numCutPointsFound] = m_planes[indPlane];
                                ++numCutPointsFound;
                                if (numCutPointsFound > 2)
                                {
                                    numCutPointsFound = 0;
                                    break;
                                }
                            }
                        }
                        if(numCutPointsFound == 0)
                        {
                            continue;
                        }
                        if(numCutPointsFound != 2)
                        {
                            continue;
                        }

                        real cutPointsSorted[2];

                        cutPointsSorted[0] = math::min<real>(cutPoints[0], cutPoints[1]);
                        cutPointsSorted[1] = math::max<real>(cutPoints[0], cutPoints[1]);
                        if (cutPointsSorted[0] != cutPoints[0])
                        {
                            std::swap(cutPlanes[0], cutPlanes[1]);
                        }
                        const real length(cutPointsSorted[1]-cutPointsSorted[0]);
                        if (length < 2.)
                        {
                            continue;
                        }
                        switch (indAxis)
                        {
                        case 0:
                            t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::x, cutPlanes[0], cutPlanes[1]);
                            break;
                        case 1:
                            t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::y, cutPlanes[0], cutPlanes[1]);
                            break;
                        case 2:
                            t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::z, cutPlanes[0], cutPlanes[1]);
                            break;
                        default:
                            BASSERT(false);
                            break;
                        }
                    }
                }
            }
        }
    }

private:
    const vector3 m_size;
    const quaternion m_rotation;

    const blub::axisAlignedBox m_aabb;

    blub::plane m_planes[6];

};


}
}
}
}


#endif // PROCEDURAL_VOXEL_EDIT_BOX_HPP
