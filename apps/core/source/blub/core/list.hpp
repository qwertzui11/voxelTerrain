#ifndef BLUB_LIST_HPP
#define BLUB_LIST_HPP

#include "blub/core/globals.hpp"

#include <boost/container/list.hpp>


namespace blub
{

template <class T>
class list : public boost::container::list<T>
{
public:
    typedef boost::container::list<T> t_base;

    list<T>() : boost::container::list<T>() {;}
    list<T>(const uint32 &num) : boost::container::list<T>() {this->reserve(num);}
    list<T>(boost::container::list<T> lst) : boost::container::list<T>(lst) {;}

    T& at(const uint32& ind)
    {
        BASSERT(ind < t_base::size());
        typename t_base::iterator it(t_base::begin());
        for (uint32 index = 0; index < ind; ++index)
        {
            ++it;
        }
        return *it;
    }

    const T& at(const uint32& ind) const
    {
        BASSERT(ind < t_base::size());
        typename t_base::const_iterator it(t_base::cbegin());
        for (uint32 index = 0; index < ind; ++index)
        {
            ++it;
        }
        return *it;
    }

    void reserve(uint32 size)
    {
        if (t_base::size() > size)
        {
            size = t_base::size();
        }
        t_base::resize(size);
    }

    bool isEmpty(void) const
    {
        return t_base::empty();
    }

    void insertAt(const uint32& ind, const T& toInsert)
    {
        BASSERT(ind < t_base::size());
        t_base::insert(t_base::cbegin()+ind, toInsert);
    }

    void removeAt(const uint32& ind)
    {
        BASSERT(ind < t_base::size());
        typename t_base::const_iterator it(t_base::cbegin());
        for (uint32 index = 0; index < ind; ++index)
        {
            ++it;
        }
        t_base::erase(it);
    }

    int32 indexOf(const T& toFind)
    {
        typename t_base::const_iterator it(t_base::cbegin());
        for (uint32 index = 0; index < t_base::size(); ++index)
        {
            if (*it == toFind)
            {
                return index;
            }
            ++it;
        }
        return -1;
    }

    /*
    T takeAt(uint32 ind)
    {
        T result = *(this->begin()+ind);
        removeAt(ind);
        return result;
    }

    T takeFirst()
    {
        T result = *this->begin();
        removeAt(0);
        return result;
    }*/

    bool push_back_distinct(const T & ins)
    {
        if (t_base::indexOf(ins) == -1)
        {
            t_base::push_back(ins);
            return true;
        }
        else
            return false;
    }
};

}

#endif // BLUB_LIST_HPP
