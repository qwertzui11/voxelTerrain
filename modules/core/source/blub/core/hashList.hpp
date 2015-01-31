#ifndef BLUB_HASHLIST_HPP
#define BLUB_HASHLIST_HPP

#include <blub/core/globals.hpp>

#include <unordered_set>
#include <utility>
#include <boost/functional/hash.hpp>


namespace blub
{


template <class Key,
          class Hash,
          class KeyEqual,
          class Allocator>
class hashList : public std::unordered_set<Key, Hash, KeyEqual, Allocator>
{

};


}


#endif // BLUB_HASHLIST_HPP
