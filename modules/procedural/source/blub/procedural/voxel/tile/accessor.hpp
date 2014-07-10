#ifndef PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP

#include "blub/core/array.hpp"
#include "blub/core/globals.hpp"
#include "blub/math/vector2int32.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


/**
 * @brief The accessor class caches all voxel needed by tile::surface for an extremly optimized and fast calculation for the same.
 * This class looks up all voxel needed for the modified marching cubes and if lod is enabled 6*arrays for every side of the cube (transvoxel).
 * See http://www.terathon.com/voxels/ and http://www.terathon.com/lengyel/Lengyel-VoxelTerrain.pdf
 */
template <class voxelType>
class accessor : public base<accessor<voxelType> >
{
public:
    typedef base<accessor<voxelType> > t_base;
    typedef tile::container<voxelType> t_voxelContainer;

    static constexpr int32 voxelLength = t_voxelContainer::voxelLength;
    static constexpr int32 voxelLengthWithNormalCorrection = voxelLength+3;
    static constexpr int32 voxelLengthLod = (voxelLength+1)*2;
    static constexpr int32 voxelCount = voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection;
    static constexpr int32 voxelCountLod = voxelLengthLod*voxelLengthLod;
    static constexpr int32 voxelCountLodAll = 6*voxelCountLod;
    static constexpr int32 voxelLengthSurface = t_voxelContainer::voxelLength+1;
    static constexpr int32 voxelCountSurface = voxelLengthSurface*voxelLengthSurface*voxelLengthSurface;

    typedef array<voxelType, voxelCount> t_voxelArray;
    typedef array<voxelType, 6*voxelCountLod> t_voxelArrayLod;


    /**
     * @brief create creates an instance.
     * @return Never nullptr.
     */
    static typename t_base::pointer create()
    {
        return typename t_base::pointer(new accessor());
    }

    /**
     * @brief ~accessor destructor.
     */
    virtual ~accessor()
    {
        if (m_calculateLod)
        {
            delete m_voxelsLod;
            m_voxelsLod = nullptr;
        }
    }

    /**
     * @brief setVoxel set ancalculateIndex convertes a 3d voxel-pos to a 1d array-index.
     * @param pos -1 <= pos.xyz < voxelLengthWithNormalCorrection-1
     * @param toSet
     * @return returns true if anything changed.
     */
    bool setVoxel(const vector3int32& pos, const voxelType& toSet)
    {
        BASSERT(pos.x >= -1);
        BASSERT(pos.y >= -1);
        BASSERT(pos.z >= -1);
        BASSERT(pos.x < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.y < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.z < voxelLengthWithNormalCorrection-1);

        if (toSet.getInterpolation() >= 0 && pos >= vector3int32(0) && pos < vector3int32(voxelLengthSurface))
        {
            ++m_numVoxelLargerZero;
        }

        const int32 index((pos.x+1)*voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection + (pos.y+1)*voxelLengthWithNormalCorrection + pos.z+1);
        const voxelType oldValue(m_voxels[index]);
        m_voxels[index] = toSet;

        return oldValue != toSet;
    }

    /**
     * @brief setVoxelLod sets a voxel to a lod array.
     * @param pos
     * @param toSet
     * @param lod level of detail index
     * @return returns true if anything changed.
     * @see calculateCoordsLod()
     */
    bool setVoxelLod(const vector3int32& pos, const voxelType& toSet, const int32& lod)
    {
        return setVoxelLod(calculateCoordsLod(pos, lod), toSet, lod);
    }

    /**
     * @brief getVoxel return ref to voxel.
     * @param pos -1 <= pos.xyz < voxelLengthWithNormalCorrection-1
     * @return
     */
    const voxelType& getVoxel(const vector3int32& pos) const
    {
        BASSERT(pos.x >= -1);
        BASSERT(pos.y >= -1);
        BASSERT(pos.z >= -1);
        BASSERT(pos.x < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.y < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.z < voxelLengthWithNormalCorrection-1);

        const int32 index((pos.x+1)*voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection + (pos.y+1)*voxelLengthWithNormalCorrection + pos.z+1);
        return m_voxels[index];
    }
    /**
     * @brief getVoxelLod returns ref to lod-voxel
     * @param pos
     * @param lod level of detail index
     * @return
     * @see calculateCoordsLod()
     */
    const voxelType& getVoxelLod(const vector3int32& pos, const int32& lod) const
    {
        return getVoxelLod(calculateCoordsLod(pos, lod), lod);
    }

    /**
     * @brief isEmpty returns true if all voxel are minimum.
     * @return
     * @see procedural::voxel::data
     */
    bool isEmpty() const
    {
        return m_numVoxelLargerZero == 0;
    }

    /**
     * @brief isFull returns true if all voxel are maximum.
     * @return
     * @see procedural::voxel::data
     */
    bool isFull() const
    {
        return m_numVoxelLargerZero == voxelCountSurface;
    }

    /**
     * @brief getCalculateLod returns true if surface later shall calculate level-of-detail
     * @return
     */
    const bool& getCalculateLod() const
    {
        return m_calculateLod;
    }

    /**
     * @brief getNumVoxelLargerZero returns number of voxel not minimum.
     * @return
     * @see procedural::voxel::data
     */
    const int32& getNumVoxelLargerZero() const
    {
        return m_numVoxelLargerZero;
    }

    /**
     * @brief getNumVoxelLargerZeroLod returns number of voxel in lod not minimum.
     * @return
     */
    const int32& getNumVoxelLargerZeroLod() const
    {
        return m_numVoxelLargerZeroLod;
    }

    /**
     * @brief getVoxelArray return voxel-array
     * @return
     */
    const t_voxelArray& getVoxelArray() const
    {
        return m_voxels;
    }
    /**
     * @brief getVoxelArray return voxel-array
     * @return
     */
    t_voxelArray& getVoxelArray()
    {
        return m_voxels;
    }
    /**
     * @brief getVoxelArrayLod returns nullptr if no lod shall get calculated else 6-lod-arrays of voxels.
     * @return
     * @see getCalculateLod()
     */
    t_voxelArrayLod* getVoxelArrayLod() const
    {
        return m_voxelsLod;
    }
    /**
     * @brief getVoxelArrayLod returns nullptr if no lod shall get calculated else 6-lod-arrays of voxels.
     * @return returns
     * @see getCalculateLod()
     */
    t_voxelArrayLod* getVoxelArrayLod()
    {
        return m_voxelsLod;
    }

    /**
     * @brief setCalculateLod enables or disables lod calculation and voxel buffering for it.
     * @param lod
     */
    void setCalculateLod(const bool& lod)
    {
        if (m_calculateLod == lod)
        {
            // do nothing
//            BWARNING("m_calculateLod == lod");
            return;
        }
        m_calculateLod = lod;
        if (m_calculateLod)
        {
            BASSERT(m_voxelsLod == nullptr);
            m_voxelsLod = new t_voxelArrayLod(); // [6*voxelCountLod]
        }
        else
        {
            delete m_voxelsLod;
            m_voxelsLod = nullptr;
        }
    }

    /**
     * @brief setNumVoxelLargerZero internally used for extern sync. (optimisation)
     * @param toSet
     */
    void setNumVoxelLargerZero(const int32& toSet)
    {
        m_numVoxelLargerZero = toSet;
    }
    /**
     * @brief setNumVoxelLargerZero internally used for extern sync. (optimisation)
     * @param toSet
     */
    void setNumVoxelLargerZeroLod(const int32& toSet)
    {
        m_numVoxelLargerZeroLod = toSet;
    }

protected:
    /**
     * @brief accessor constructor
     */
    accessor()
        : m_voxelsLod(nullptr)
        , m_calculateLod(false)
        , m_numVoxelLargerZero(0)
        , m_numVoxelLargerZeroLod(0)
    {
    }

    /**
     * @see setVoxelLod()
     */
    bool setVoxelLod(const vector2int32& index, const voxelType& toSet, const int32& lod)
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCountLod);
        BASSERT(m_calculateLod);
        BASSERT(m_voxelsLod != nullptr);

        if (toSet.getInterpolation() >= 0)
        {
            ++m_numVoxelLargerZeroLod;
        }
        const uint32 index_(lod*voxelCountLod + index.x*voxelLengthLod + index.y);
        const voxelType oldValue((*m_voxelsLod)[index_]);
        (*m_voxelsLod)[index_] = toSet;

        return oldValue != toSet;
    }
    /**
     * @see getVoxelLod()
     */
    const voxelType& getVoxelLod(const vector2int32& index, const int32& lod) const
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCountLod);
        BASSERT(m_calculateLod);
        return (*m_voxelsLod)[lod*voxelCountLod + index.x*voxelLengthLod + index.y];
    }

    /**
     * @brief calculateCoordsLod converts a 3d lod-coord to a 2d. Transvoxel needs per quader-side (6) only one 2d half-size/detail array.
     * See http://www.terathon.com/voxels/ and http://www.terathon.com/lengyel/Lengyel-VoxelTerrain.pdf
     * @param pos
     * @param lod
     * @return
     */
    static vector2int32 calculateCoordsLod(const vector3int32& pos, const int32& lod)
    {
        BASSERT(lod >= 0);
        BASSERT(lod < 6);
        BASSERT(pos.x >= 0);
        BASSERT(pos.y >= 0);
        BASSERT(pos.z >= 0);
        BASSERT(pos.x < voxelLengthLod);
        BASSERT(pos.y < voxelLengthLod);
        BASSERT(pos.z < voxelLengthLod);
        BASSERT(pos.x == 0 || pos.y == 0 || pos.z == 0); // 2d!

        switch (lod)
        {
        case 0: // x
        case 1:
            BASSERT(pos.x == 0);
            return vector2int32(pos.y, pos.z);
        case 2: // y
        case 3:
            BASSERT(pos.y == 0);
            return vector2int32(pos.x, pos.z);
        case 4: // z
        case 5:
            BASSERT(pos.z == 0);
            return vector2int32(pos.x, pos.y);
        default:
            BASSERT(false);
        }
        return vector2int32();
    }

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        using namespace serialization;

        (void)version;

        readWrite & nameValuePair::create("numVoxelLargerZero", m_numVoxelLargerZero);
        readWrite & nameValuePair::create("numVoxelLargerZeroLod", m_numVoxelLargerZeroLod);
        readWrite & nameValuePair::create("calculateLod", m_calculateLod);

        readWrite & nameValuePair::create("voxels", m_voxels); // OPTIMISE gives twice the size in binary format (2 instead of 1)

        if (m_calculateLod)
        {
            if (m_voxelsLod == nullptr)
            {
                setCalculateLod(m_calculateLod);
            }
            BASSERT(m_voxelsLod != nullptr);
            readWrite & nameValuePair::create("voxelsLod", *m_voxelsLod);
        }
    }



private:
    t_voxelArray m_voxels;
    t_voxelArrayLod *m_voxelsLod;

    bool m_calculateLod;
    int32 m_numVoxelLargerZero;
    int32 m_numVoxelLargerZeroLod;


};


template <class voxelType>
constexpr int32 accessor<voxelType>::voxelLength;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelLengthWithNormalCorrection;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelLengthLod;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelCount;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelCountLod;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelCountLodAll;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelLengthSurface;
template <class voxelType>
constexpr int32 accessor<voxelType>::voxelCountSurface;


}
}
}
}




#endif // PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP
