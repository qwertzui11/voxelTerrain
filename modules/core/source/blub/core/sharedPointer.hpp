#ifndef BLUB_SHAREDPOINTER_HPP
#define BLUB_SHAREDPOINTER_HPP

#include <memory>


namespace blub
{


template <class T>
class sharedPointer : public std::shared_ptr<T>
{
public:
    typedef std::shared_ptr<T> t_base;

    sharedPointer()
    {}
    sharedPointer(T* ptr)
        : t_base(ptr)
    {}
    sharedPointer(t_base ptr)
        : t_base(ptr)
    {}
    template <class U>
    sharedPointer(std::shared_ptr<U> ptr)
        : t_base(ptr)
    {}

    template <class U>
    sharedPointer<U> staticCast()
    {
        std::shared_ptr<U> result(std::static_pointer_cast<U>(*this));
        return result;
    }

    T* data() const
    {
        return t_base::get();
    }

    void reset()
    {
        t_base::reset();
    }

    bool isNull() const
    {
        return data() == nullptr;
    }

protected:

};


using std::make_shared;


}


#endif // BLUB_SHAREDPOINTER_HPP
