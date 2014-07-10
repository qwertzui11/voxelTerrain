#ifndef BLUB_SHAREDPOINTER_HPP
#define BLUB_SHAREDPOINTER_HPP

#include <boost/shared_ptr.hpp>


namespace blub
{


template <class T>
class sharedPointer : public boost::shared_ptr<T>
{
public:
    sharedPointer()
        : boost::shared_ptr<T>()
    {}
    sharedPointer(T* ptr)
        : boost::shared_ptr<T>(ptr)
    {}
    sharedPointer(boost::shared_ptr<T> ptr)
        : boost::shared_ptr<T>(ptr)
    {}
    template <class U>
    sharedPointer(boost::shared_ptr<U> ptr)
        : boost::shared_ptr<T>(ptr)
    {}

    template <class U>
    sharedPointer<U> staticCast()
    {
        boost::shared_ptr<U> result(boost::static_pointer_cast<U>(*this));
        return result;
    }

    T* data() const
    {
        return boost::shared_ptr<T>::get();
    }

    void reset()
    {
        boost::shared_ptr<T>::reset();
    }

    bool isNull() const
    {
        return data() == nullptr;
    }

protected:

};

}


#endif // BLUB_SHAREDPOINTER_HPP
