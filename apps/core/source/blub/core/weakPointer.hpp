#ifndef WEAKPOINTER_HPP
#define WEAKPOINTER_HPP

#include "blub/core/globals.hpp"

#include <boost/weak_ptr.hpp>


namespace blub
{


template <class T>
class weakPointer : public boost::weak_ptr<T>
{
public:
    weakPointer() : boost::weak_ptr<T>() {}

    template<class Y> weakPointer(sharedPointer<Y> const & r) : boost::weak_ptr<T>(r) {}

    weakPointer(weakPointer const & r) : boost::weak_ptr<T>(r) {}
    template<class Y> weakPointer(weakPointer<Y> const & r) : boost::weak_ptr<T>(r) {}

    template <class U>
    sharedPointer<U> staticCast(void)
    {
        boost::shared_ptr<U> result(boost::static_pointer_cast<U>(*this));
        return result;
    }

    T* data() const
    {
        return boost::shared_ptr<T>::get();
    }

    bool isNull() const
    {
        return data() == nullptr;
    }

protected:
};

}


#endif // WEAKPOINTER_HPP
