#ifndef SERIALIZATION_SAVELOAD_HPP
#define SERIALIZATION_SAVELOAD_HPP

#include <boost/serialization/split_member.hpp>


#define BLUB_SERIALIZATION_SAVELOAD() BOOST_SERIALIZATION_SPLIT_MEMBER()


namespace blub
{
namespace serialization
{


template<class Archive, class T>
void saveLoad(Archive & ar, T & t, const unsigned int file_version)
{
    boost::serialization::split_member(ar, t, file_version);
}


}
}


#endif // SERIALIZATION_SAVELOAD_HPP
