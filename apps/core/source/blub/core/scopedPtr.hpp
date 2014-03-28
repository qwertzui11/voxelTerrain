#ifndef BLUB_CORE_SCOPEDPTR_HPP
#define BLUB_CORE_SCOPEDPTR_HPP

#include <boost/scoped_ptr.hpp>


namespace blub
{

template <class T>
class scopedPointer : public boost::scoped_ptr<T>
{
public:
    typedef boost::scoped_ptr<T> t_base;

    scopedPointer( T * p = 0)
        : t_base(p)
    {
        ;
    }

    bool isNull() const
    {
        return (t_base::get() == nullptr);
    }

};

}


#endif // BLUB_CORE_SCOPEDPTR_HPP
