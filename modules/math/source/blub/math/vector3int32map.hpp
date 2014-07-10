#ifndef VECTOR3INT32MAP_HPP
#define VECTOR3INT32MAP_HPP

#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/core/noncopyable.hpp"


namespace blub
{

template <typename dataType>
class vector3int32map : public noncopyable
{
public:
    vector3int32map()
        : m_data(nullptr)
    {
        ;
    }

    ~vector3int32map()
    {
        if (m_data != nullptr)
        {
            delete [] m_data;
        }
    }

    void setValue(const vector3int32& pos, const dataType& toSet)
    {
        const int32 index(convertToIndex(pos));
        BASSERT(index < m_bounds.getVolume());
        m_data[index] = toSet;
    }
    const dataType& getValue(const vector3int32& pos) const
    {
        const int32 index(convertToIndex(pos));
        return m_data[index];
    }


    void resize(const axisAlignedBoxInt32& aabb)
    {
        axisAlignedBoxInt32 resizedAabb(m_bounds);
        resizedAabb.extend(aabb);
        if (resizedAabb == m_bounds)
        {
            return;
        }
        const axisAlignedBoxInt32 oldBounds(m_bounds);
        const vector3int32 oldStart(oldBounds.getMinimum());
        const vector3int32 oldEnd(oldBounds.getMaximum());
        const dataType* oldData(m_data);

        m_bounds = resizedAabb;
        BASSERT(m_bounds.getVolume() > 0);
        m_data = new dataType[m_bounds.getVolume()];

        if (oldData != nullptr)
        {
            for (int32 indX = oldStart.x; indX < oldEnd.x; ++indX)
            {
                for (int32 indY = oldStart.y; indY < oldEnd.y; ++indY)
                {
                    for (int32 indZ = oldStart.z; indZ < oldEnd.z; ++indZ)
                    {
                        m_data[convertToIndex(vector3int32(indX, indY, indZ))] = oldData[convertToIndex(vector3int32(indX, indY, indZ), oldBounds)];
                    }
                }
            }
            delete [] oldData;
        }
    }

    void extend(const vector3int32& toExtend)
    {
        axisAlignedBoxInt32 resizedAabb(m_bounds);
        resizedAabb.extend(toExtend);
        resize(resizedAabb);
    }
    void extend(const axisAlignedBoxInt32 &toExtend)
    {
        axisAlignedBoxInt32 resizedAabb(m_bounds);
        resizedAabb.extend(toExtend);
        resize(resizedAabb);
    }

    const axisAlignedBoxInt32& getBounds(void) const
    {
        return m_bounds;
    }

protected:

    int32 convertToIndex(const vector3int32& toConvert) const
    {
        return convertToIndex(toConvert, m_bounds);
    }
    static int32 convertToIndex(const vector3int32& toConvert, const axisAlignedBoxInt32& bounds)
    {
        BASSERT(toConvert >= bounds.getMinimum());
        BASSERT(toConvert < bounds.getMaximum());

        const vector3int32 size(bounds.getSize());
        const vector3int32 min(bounds.getMinimum());
        return (toConvert.x-min.x)*size.y*size.z + (toConvert.y-min.y)*size.z + (toConvert.z-min.z);
    }

    dataType *m_data;
    axisAlignedBoxInt32 m_bounds;


};

}


#endif // VECTOR3INT32MAP_HPP
