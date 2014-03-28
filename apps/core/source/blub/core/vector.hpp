#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "blub/core/globals.hpp"
#include "blub/serialization/access.hpp"
#include "blub/serialization/nameValuePair.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>


namespace blub
{

    template <typename T>
    class vector : public std::vector<T>
    {
    public:
        typedef std::vector<T> t_base;

        vector() {;}
        vector(const std::size_t& size)
            : t_base(size)
        {
        }
        vector(const std::initializer_list<T> &list)
            : t_base(list)
        {
        }
    protected:
        BLUB_SERIALIZATION_ACCESS
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            (void)version;
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(t_base); // TODO remove boost macro, use blub
        }
    };

    template <typename T>
    std::ostream& operator<< (std::ostream& ostr, const vector<T>& toCast)
    {
        ostr << "{";
        for (const T& elem : toCast)
        {
            ostr << elem << ",";
        }
        return ostr << "}";
    }

}


#endif // VECTOR_HPP
