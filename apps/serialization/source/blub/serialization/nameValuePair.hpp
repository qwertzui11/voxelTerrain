#ifndef SERIALIZATION_NAMEVALUEPAIR_HPP
#define SERIALIZATION_NAMEVALUEPAIR_HPP

#include "blub/core/globals.hpp"

#include <boost/serialization/nvp.hpp>




namespace blub
{
namespace serialization
{


class nameValuePair
{
public:
    template<class T>
    static inline
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    const
#endif
    boost::serialization::nvp< T > create(const char * name, T & t){
        return boost::serialization::nvp< T >(name, t);
    }

};


}
}

#define BLUB_SERIALIZATION_NAMEVALUEPAIR(x) BOOST_SERIALIZATION_NVP(x)



#endif // SERIALIZATION_NAMEVALUEPAIR_HPP
