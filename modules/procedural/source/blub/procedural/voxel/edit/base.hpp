#ifndef BLUB_PROCEDURAL_VOXEL_EDIT_BASE_HPP
#define BLUB_PROCEDURAL_VOXEL_EDIT_BASE_HPP


#include "blub/core/enableSharedFromThis.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/plane.hpp"
#include "blub/math/transform.hpp"
#include "blub/math/vector3int.hpp"
#include "blub/procedural/predecl.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
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
 * @brief base class of all edits. Has various features you may wanna overwrite.
 * If you want to write your own edit overwrite the method calculateOneVoxel() or calculateVoxel(). For most flexibility override calculateVoxel().
 * Because base derives enableSharedFromThis you have to write a create method that creates a shared_ptr of the class.
 */
template <class configType>
class base : public enableSharedFromThis<base<configType> >, public blub::noncopyable
{
public:
    typedef configType t_config;
    typedef enableSharedFromThis<base<t_config> > t_base;
    typedef sharedPointer<base<t_config> > pointer;
    typedef typename t_config::t_container::t_simple t_voxelContainerSimple;
    typedef typename t_config::t_container::t_tile t_voxelContainerTile;
    typedef typename t_config::t_data t_voxel;

    /**
     * @brief destructor
     */
    virtual ~base()
    {
    }

    /**
     * @brief calculates voxel in the container u have to set by setVoxelContainer() before.
     * If setVoxelContainer() didn't get called before, method will assert.
     * @param trans The transform used on every voxel position before calling calculateOneVoxel().
     */
    void calculateVoxel(const transform &trans) const
    {
        BASSERT(m_voxelContainer != nullptr);

        m_voxelContainer->editVoxel(t_base::getSharedThisPtr(), trans);
    }

    /**
     * @brief calculates all voxel in getAxisAlignedBoundingBox() and inserts them into voxelContainer.
     * Method gets called once per tile from various threads.
     * Voxel only will get set if the interpolation is higher than the inerpoaltion before calculation.
     * @param voxelContainer The container where the voxel have to get set in.
     * @param voxelContainerOffset The absolut offset of the container
     * @param trans The transform of the edit.
     */
    virtual void calculateVoxel(t_voxelContainerTile* voxelContainer,
                                const vector3int32& voxelContainerOffset,
                                const transform &trans) const
    {
#ifdef BLUB_LOG_VOXEL
        BLUB_PROCEDURAL_LOG_OUT() << "calculateVoxel trans:" << trans;
#endif

        const blub::axisAlignedBox aabb(getAxisAlignedBoundingBox(trans));

        const int32 voxelsPerTile(t_config::voxelsPerTile);
        const vector3int32 posContainerAbsolut(voxelContainerOffset*voxelsPerTile);
        for (int32 indX = 0; indX < t_config::voxelsPerTile; ++indX)
        {
            for (int32 indY = 0; indY < t_config::voxelsPerTile; ++indY)
            {
                for (int32 indZ = 0; indZ < t_config::voxelsPerTile; ++indZ)
                {
                    const vector3int32 posVoxel(indX, indY, indZ);
                    vector3 posAbsolut(posContainerAbsolut + posVoxel);

                    if (!aabb.contains(posAbsolut))
                    {
                        continue;
                    }

                    posAbsolut -= trans.position;
                    posAbsolut /= trans.scale;

                    t_voxel voxelResult;
                    const bool changeVoxel(calculateOneVoxel(posAbsolut, &voxelResult));

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
                        voxelResult.getInterpolation() *= -1;
                        voxelContainer->setVoxelIfInterpolationLower(posVoxel, voxelResult);
                    }
                }
            }
        }
    }

    /**
     * @brief Enable or disable cut. Don't change the value while active calculation.
     * @param cut If true voxel will get removed instead in added.
     */
    void setCut(const bool& cut)
    {
        m_cut = cut;
    }
    /**
     * @brief Returns if cutting is enabled.
     * @return Returns the setted value by setCut() or default false.
     */
    const bool &getCut() const
    {
        return m_cut;
    }

    /**
     * @brief Sets the voxelcontainer used by calculateVoxel().
     * @param toSet Must not be nullptr.
     */
    void setVoxelContainer(t_voxelContainerSimple* toSet)
    {
        m_voxelContainer = toSet;
    }
    /**
     * @return Returns voxelContainer set by setVoxelContainer()
     */
    t_voxelContainerSimple* getVoxelContainer() const
    {
        return m_voxelContainer;
    }

    /**
     * @brief getAxisAlignedBoundingBox returns the transformed blub::axisAlignedBox in which the voxel have to get recalculated.
     * @param trans
     * @return
     */
    virtual blub::axisAlignedBox getAxisAlignedBoundingBox(const transform& trans) const = 0;

protected:
    base()
        : m_voxelContainer(nullptr)
        , m_cut(false)
    {
    }

    /**
     * @brief Implement this method for your own edit
     * @param pos describes the voxel-position
     * @param resultVoxel if a value could get calculated, set it to resultVoxel.
     * @return true if it was possible to calculate a value.
     */
    virtual bool calculateOneVoxel(const vector3& pos, t_voxel* resultVoxel) const
    {
        return false;
    }

    /**
     * @brief The axis enum is used by createLine() for describing the direction.
     */
    enum class axis
    {
        x,
        y,
        z
    };

    /**
     * @brief createLine creates a voxel-line on one axis with a specific length
     * @param voxelContainer Where to create the line in. Must not be nullptr
     * @param posVoxel The relative voxel position in 2d. The value definied by ax will be ignored.
     * @param from Start value from where to start the line. Not a an integer because of interpolation.
     * @param len length value added to start of the line. Not a an integer because of interpolation. Must be larger/equal zero
     * @param ax The axis on which the line should get generated
     * @param planeA first cutPlane. See blub::procedural::voxel::edit::box for usage. Used for interpolation.
     * @param planeB second cutPlane. See blub::procedural::voxel::edit::box for usage. Used for interpolation.
     */
    virtual void createLine(t_voxelContainerTile *voxelContainer,
                            const vector3int32 &posVoxel,
                            const real &from,
                            const real &len,
                            const axis &ax,
                            const blub::plane &planeA,
                            const blub::plane &planeB) const
    {
        const real &pointA(from);
        real pointB;
        vector3int32 lineAxis;
        switch (ax)
        {
        case axis::x:
            pointB = pointA+len;
            lineAxis = vector3int32(1, 0, 0);
            break;
        case axis::y:
            pointB = pointA+len;
            lineAxis = vector3int32(0, 1, 0);
            break;
        case axis::z:
            pointB = pointA+len;
            lineAxis = vector3int32(0, 0, 1);
            break;
        default:
            BASSERT(false);
        }

        t_voxel bufferVoxel;

        int32 voxelStartRel(0);
        int32 voxelEndRel(0);
        const real level(0.5); // cos(60 Degree)
        {
            {
                real roundedDown(blub::math::floor(pointA));
                if (roundedDown >= 0 && roundedDown < t_config::voxelsPerTile)
                {
                    if (vector3(lineAxis).dotProduct(planeA.normal) <= -level)
                    {
                        real valueDown(roundedDown-pointA);
                        real zResultDown(127.*valueDown);
                        BASSERT(zResultDown <= 0.);
                        BASSERT(zResultDown >= -127.);
                        bufferVoxel.setInterpolation(zResultDown);
                        setVoxel(voxelContainer, posVoxel + vector3int32(roundedDown)*lineAxis, bufferVoxel);
                    }
                    ++voxelStartRel;
                }
            }
            {
                real roundedUp(blub::math::ceil(pointA));
                if (roundedUp >= 0. && roundedUp < t_config::voxelsPerTile)
                {
                    if (vector3(lineAxis).dotProduct(planeA.normal) <= -level)
                    {
                        real valueUp(roundedUp-pointA);
                        real zResultUp(127.*valueUp);
                        BASSERT(zResultUp >= 0.);
                        BASSERT(zResultUp <= 127.);
                        bufferVoxel.setInterpolation(zResultUp);
                        setVoxel(voxelContainer, posVoxel + vector3int32(roundedUp)*lineAxis, bufferVoxel);
                    }
                    ++voxelStartRel;
                }
            }
        }
        {
            {
                real roundedDown(blub::math::floor(pointB));
                if (roundedDown >= 0 && roundedDown < t_config::voxelsPerTile)
                {
                    if (vector3(lineAxis).dotProduct(planeB.normal) >= level)
                    {
                        real valueDown(roundedDown-pointB);
                        real zResultDown(127.*valueDown);
                        BASSERT(zResultDown <= 0.);
                        BASSERT(zResultDown >= -127.);
                        bufferVoxel.setInterpolation(-zResultDown);
                        setVoxel(voxelContainer, posVoxel + vector3int32(roundedDown)*lineAxis, bufferVoxel);
                    }
                    ++voxelEndRel;
                }
            }
            {
                real roundedUp(blub::math::ceil(pointB));
                if (roundedUp >= 0. && roundedUp < t_config::voxelsPerTile)
                {
                    if (vector3(lineAxis).dotProduct(planeB.normal) >= level)
                    {
                        real valueUp(roundedUp-pointB);
                        real zResultUp(127.*valueUp);
                        BASSERT(zResultUp >= 0.);
                        BASSERT(zResultUp <= 127.);
                        bufferVoxel.setInterpolation(-zResultUp);
                        setVoxel(voxelContainer, posVoxel + vector3int32(roundedUp)*lineAxis, bufferVoxel);
                    }
                    ++voxelEndRel;
                }
            }
        }
        {
            real cutPointVoxelDown = blub::math::floor(pointA);
            real cutPointVoxelUp = blub::math::ceil(pointB+1);

            int32 voxelClampedStart = math::clamp<real>(cutPointVoxelDown, 0., t_config::voxelsPerTile);
            int32 voxelClampedEnd = math::clamp<real>(cutPointVoxelUp, 0., t_config::voxelsPerTile);

            for (int32 ind = voxelClampedStart + voxelStartRel; ind < voxelClampedEnd - voxelEndRel; ++ind)
            {
                bufferVoxel.setInterpolation(0);
                setVoxel(voxelContainer, posVoxel + vector3int32(ind)*lineAxis, bufferVoxel);
            }
        }
    }


private:
    virtual void setVoxel(t_voxelContainerTile *voxelContainer, const vector3int32 &posVoxel, t_voxel &toSet) const
    {
        voxelContainer->setVoxelIfInterpolationHigher(posVoxel, toSet);
    }

protected:
    t_voxelContainerSimple*  m_voxelContainer;

    bool m_cut;

};


}
}
}
}


#endif // BLUB_PROCEDURAL_VOXEL_EDIT_BASE_HPP
