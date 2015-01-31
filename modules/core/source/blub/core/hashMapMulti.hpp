#ifndef HASHMULTIMAP_HPP
#define HASHMULTIMAP_HPP

#include "blub/core/pair.hpp"

#include <boost/unordered_map.hpp>


namespace blub
{

template <class S,
    class T,
    class H,// = boost::hash<S>,
    class P,// = std::equal_to<S>,
    class A// = std::allocator<pair<const S, T> >
          >
class hashMapMulti : public boost::unordered::unordered_multimap<S, T, H, P, A>
{
public:
    typedef boost::unordered::unordered_multimap<S, T, H, P, A> t_base;
    typedef std::pair<typename t_base::iterator, typename t_base::iterator> equal_range_result;
    typedef std::pair<typename t_base::const_iterator, typename t_base::const_iterator> equal_range_result_const;


    hashMapMulti<S, T, H, P, A>() : t_base() {;}


    typename t_base::const_iterator constBegin() const
    {
        return t_base::cbegin();
    }

    typename t_base::const_iterator constEnd() const
    {
        return t_base::cend();
    }

    void insert(const typename t_base::key_type& key, const typename t_base::mapped_type& value)
    {
        blub::pair<typename t_base::key_type, typename t_base::mapped_type> toInsert(key, value);
        t_base::insert(toInsert);
    }

    void remove(const typename t_base::key_type& key)
    {
        t_base::erase(key);
    }

    bool contains(const typename t_base::key_type& key) const
    {
        return t_base::find(key) != t_base::cend();
    }

};

}


#endif // HASHMULTIMAP_HPP
