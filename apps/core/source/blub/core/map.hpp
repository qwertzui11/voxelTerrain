#ifndef MAP_HPP
#define MAP_HPP

#include <map>


namespace blub
{

template <class S, class T>
class map : public std::map<S, T>
{
public:
    typedef std::map<S, T> t_base;

    map<S, T>() : t_base() {;}

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

#endif // MAP_HPP
