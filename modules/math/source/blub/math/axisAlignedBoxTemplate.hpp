#ifndef AXISALIGNEDBOXTEMPLATE_HPP
#define AXISALIGNEDBOXTEMPLATE_HPP

#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"

#include "blub/core/globals.hpp"


namespace blub
{


template <typename vector3Type>
class axisAlignedBoxTemplate
{
public:
    typedef axisAlignedBoxTemplate<vector3Type> t_thisClass;

    axisAlignedBoxTemplate()
    {

    }

    axisAlignedBoxTemplate(const vector3Type& min, const vector3Type& max)
        : m_min(min)
        , m_max(max)
    {
        ;
    }

    bool operator == (const t_thisClass& other) const
    {
        return other.getMinimum() == m_min && other.getMaximum() == m_max;
    }

    void setMinimumAndMaximum(const vector3Type& min, const vector3Type& max)
    {
        m_min = min;
        m_max = max;
    }

    const vector3Type& getMinimum(void) const
    {
        return m_min;
    }

    const vector3Type& getMaximum(void) const
    {
        return m_max;
    }

    vector3Type getSize(void) const
    {
        return m_max - m_min;
    }

    vector3Type getCenter(void) const
    {
        return (m_min + m_max) / 2;
    }

    bool isInside(const vector3Type& in) const
    {
        return in >= m_min && in <= m_max;
    }

    bool isInside(const t_thisClass& in) const
    {
        return in.isInside(m_min) && in.isInside(m_max);
    }

    void extend(const t_thisClass& ext)
    {
        if (!ext.isValid())
        {
            return;
        }
        extend(ext.getMinimum());
        extend(ext.getMaximum());
    }
    void extend(const vector3Type& ext)
    {
        if (isValid())
        {
            setMinimumAndMaximum(m_min.getMinimum(ext), m_max.getMaximum(ext));
            return;
        }
        setMinimumAndMaximum(ext, ext);
    }

    bool isValid() const
    {
        return m_max >= m_min;
    }

    typename vector3Type::t_valueType getVolume()
    {
        const vector3Type size(getSize());
        return size.x * size.y * size.z;
    }


private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void serialize(formatType & readWrite, const uint32& version)
    {
        (void)version;

        readWrite & serialization::nameValuePair::create("min", m_min);
        readWrite & serialization::nameValuePair::create("max", m_max);
    }


protected:
    vector3Type m_min;
    vector3Type m_max;

};


}


#endif // AXISALIGNEDBOXTEMPLATE_HPP
