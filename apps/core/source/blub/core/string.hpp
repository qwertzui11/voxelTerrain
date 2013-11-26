#ifndef BLUBSTRING_HPP
#define BLUBSTRING_HPP

#include "blub/core/globals.hpp"

#include <boost/lexical_cast.hpp>


namespace blub
{

    class string : public std::string
    {
    public:
        typedef std::string t_base;

        // qt string
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
        /* depricated string(const string& str)
            : t_base(str)
        {}*/
        string(const byteArray& array);

/*#ifndef NO_OGRE3D
        // Ogre3D strings
        inline string(const Ogre::String& str);
        operator Ogre::String() const;
#endif*/

        template<typename numberType>
        static string number(const numberType& n)
        {
            return boost::lexical_cast<string>(n);
        }

        static string number (const axisAlignedBox& aab);
        static string number (const vector3int32& vec);
        static string number (const vector3& vec);
        static string number (const vector2& vec);
        static string number (const quaternion &quat);
        static string number (const transform &trans);

    };

}

#endif // BLUBSTRING_HPP
