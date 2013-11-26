#ifndef PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP

#include "blub/core/array.hpp"
#include "blub/core/globals.hpp"
#include "blub/math/vector2int32.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/data.hpp"
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


class accessor : public base<accessor>
{
public:
    typedef base<accessor> t_base;

    static constexpr int32 voxelLength = tile::container::voxelLength;
    static constexpr int32 voxelLengthWithNormalCorrection = voxelLength+3;
    static constexpr int32 voxelLengthLod = (voxelLength+1)*2;
    static constexpr int32 voxelCount = voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection;
    static constexpr int32 voxelCountLod = voxelLengthLod*voxelLengthLod;
    static constexpr int32 voxelCountLodAll = 6*voxelCountLod;
    static constexpr int32 voxelLengthSurface = tile::container::voxelLength+1;
    static constexpr int32 voxelCountSurface = voxelLengthSurface*voxelLengthSurface*voxelLengthSurface;

    typedef array<data, voxelCount> t_voxelArray;
    typedef array<data, 6*voxelCountLod> t_voxelArrayLod;


    static pointer create(const bool& calculateLod = false);
    ~accessor();

    bool setVoxel(const vector3int32& pos, const data& toSet)
    {
        BASSERT(pos.x >= -1);
        BASSERT(pos.y >= -1);
        BASSERT(pos.z >= -1);
        BASSERT(pos.x < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.y < voxelLengthWithNormalCorrection-1);
        BASSERT(pos.z < voxelLengthWithNormalCorrection-1);

        if (toSet.interpolation >= 0 && pos >= vector3int32(0) && pos < vector3int32(voxelLengthSurface))
        {
            ++m_numVoxelLargerZero;
        }

        const int32 index((pos.x+1)*voxelLengthWithNormalCorrection*voxelLengthWithNormalCorrection + (pos.y+1)*voxelLengthWithNormalCorrection + pos.z+1);
        const data oldValue(m_voxels[index]);
        m_voxels[index] = toSet;

        return oldValue != toSet;
    }

    bool setVoxelLod(const vector3int32& pos, const data& toSet, const int32& lod)
    {
        return setVoxelLod(calculateCoordsLod(pos, lod), toSet, lod);
    }

    void setEmpty(void)
    {
        setValueToAllVoxel(tile::container::voxelInterpolationMinimum);
    }

    void setFull(void)
    {
        setValueToAllVoxel(tile::container::voxelInterpolationMaximum);
    }

    const data& getVoxel(const vector3int32& pos) const
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
    const data& getVoxelLod(const vector3int32& pos, const int32& lod) const
    {
        return getVoxelLod(calculateCoordsLod(pos, lod), lod);
    }

    bool isEmpty(void) const
    {
        /*if (m_calculateLod)
        {
            return  m_numVoxelLargerZero == 0 &&
                    m_numVoxelLargerZeroLod == 0;
        }*/
        return m_numVoxelLargerZero == 0;
    }

    bool isFull(void) const
    {
        /*if (m_calculateLod)
        {
            return  m_numVoxelLargerZero == voxelCountSurface &&
                    m_numVoxelLargerZeroLod == voxelCountLodAll;
        }*/
        return m_numVoxelLargerZero == voxelCountSurface;
    }

    const bool& getCalculateLod(void) const
    {
        return m_calculateLod;
    }

    const int32& getNumVoxelLargerZero(void) const
    {
        return m_numVoxelLargerZero;
    }

    const int32& getNumVoxelLargerZeroLod(void) const
    {
        return m_numVoxelLargerZeroLod;
    }

    const t_voxelArray& getVoxelArray() const
    {
        return m_voxels;
    }
    t_voxelArray& getVoxelArray()
    {
        return m_voxels;
    }
    t_voxelArrayLod* getVoxelArrayLod() const
    {
        return m_voxelsLod;
    }
    t_voxelArrayLod* getVoxelArrayLod()
    {
        return m_voxelsLod;
    }

    void setCalculateLod(const bool& lod);

    void setNumVoxelLargerZero(const int32& toSet)
    {
        m_numVoxelLargerZero = toSet;
    }
    void setNumVoxelLargerZeroLod(const int32& toSet)
    {
        m_numVoxelLargerZeroLod = toSet;
    }

protected:
    accessor(const bool& calculateLod = false)
        : m_voxelsLod(nullptr)
        , m_calculateLod(false)
        , m_numVoxelLargerZero(0)
        , m_numVoxelLargerZeroLod(0)
    {
        if (calculateLod)
        {
            setCalculateLod(calculateLod);
        }
    }

    bool setVoxelLod(const vector2int32& index, const data& toSet, const int32& lod)
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCountLod);
        BASSERT(m_calculateLod);
        BASSERT(m_voxelsLod != nullptr);

        if (toSet.interpolation >= 0)
        {
            ++m_numVoxelLargerZeroLod;
        }
        const uint32 index_(lod*voxelCountLod + index.x*voxelLengthLod + index.y);
        const data oldValue((*m_voxelsLod)[index_]);
        (*m_voxelsLod)[index_] = toSet;

        return oldValue != toSet;
    }
    const data& getVoxelLod(const vector2int32& index, const int32& lod) const
    {
        BASSERT(index >= 0);
        BASSERT(index < voxelCountLod);
        BASSERT(m_calculateLod);
        return (*m_voxelsLod)[lod*voxelCountLod + index.x*voxelLengthLod + index.y];
    }

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

    void setValueToAllVoxel(const int8& value);

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

        readWrite & nameValuePair::create("voxels", m_voxels);

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


}
}
}
}




#endif // PROCEDURAL_VOXEL_TILE_ACCESSOR_HPP
