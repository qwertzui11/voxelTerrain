#ifndef BLUB_STRING_HPP
#define BLUB_STRING_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/serialization/binary_object.hpp>

#ifdef BLUB_USE_SOCI
#define SOCI_USE_BOOST
#include "soci/soci.h"
#endif


namespace blub
{

    class string : public std::string
    {
    public:
        typedef std::string t_base;

        string()
            : t_base()
        {;}
        string(const char* str)
            : t_base(str)
        {}
        string(const char* str, uint32 len)
            : t_base(str, len)
        {}
        string(const t_base& str)
            : t_base(str)
        {}
        string(const byteArray& array);

        template<typename numberType>
        static string number(const numberType& n)
        {
            return boost::lexical_cast<string>(n);
        }

        static string number (const axisAlignedBox& aab); // TODO remove these... use operator << in the classes.
        static string number (const vector3int32& vec);
        static string number (const vector3& vec);
        static string number (const vector2& vec);
        static string number (const quaternion &quat);
        static string number (const transform &trans);

    protected:
        BLUB_SERIALIZATION_ACCESS
        template<typename Archive>
        void serialize(Archive & readWrite, const unsigned int version)
        {
            (void)version;

            readWrite & BOOST_SERIALIZATION_BASE_OBJECT_NVP(t_base);
        }
    };

}

std::ostream& operator << (std::ostream& ostr, const blub::string& toCast);


#ifdef BLUB_USE_SOCI

namespace soci
{
    template <>
    struct type_conversion<blub::string>
    {
        typedef std::string base_type;

        static void from_base(std::string i, soci::indicator ind, blub::string & mi)
        {
            if (ind == i_null)
            {
                throw soci_error("Null value not allowed for this type");
            }

            mi = static_cast<blub::string>(i);
        }

        static void to_base(const blub::string & mi, std::string & i, indicator & ind)
        {
            i = static_cast<std::string>(mi);
            ind = i_ok;
        }
    };
}

#endif


#endif // BLUB_STRING_HPP
