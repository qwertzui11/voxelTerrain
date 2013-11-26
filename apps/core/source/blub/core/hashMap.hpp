#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <boost/unordered_map.hpp>


namespace blub
{

template <class S,
    class T,
    class H,// = boost::hash<S>,
    class P,// = std::equal_to<S>,
    class A>// = std::allocator<std::pair<const S, T> > >
class hashMap : public boost::unordered::unordered_map<S, T, H, P, A>
{
public:
    typedef boost::unordered::unordered_map<S, T, H, P, A> t_base;


    hashMap<S, T, H, P, A>() : t_base() {;}

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
        t_base::operator [](key) = value;
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

#endif // HASHMAP_HPP
