#ifndef SHAREDPOINTER_HPP
#define SHAREDPOINTER_HPP

#include <boost/shared_ptr.hpp>


namespace blub
{


template <class T>
class sharedPointer : public boost::shared_ptr<T>
{
public:
    sharedPointer() : boost::shared_ptr<T>() {}
    sharedPointer(T* ptr) : boost::shared_ptr<T>(ptr) {}
    sharedPointer(boost::shared_ptr<T> ptr) : boost::shared_ptr<T>(ptr) {}

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


#endif // SHAREDPOINTER_HPP
