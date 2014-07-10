#ifndef HASHLIST_HPP
#define HASHLIST_HPP

#include <boost/unordered_set.hpp>

namespace blub
{

template <class key>
class hashList : public boost::unordered_set<key>
{

};


}


#endif // HASHLIST_HPP
