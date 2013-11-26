#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <boost/thread/mutex.hpp>


namespace blub
{
namespace async
{


class mutex : public boost::mutex
{
public:
    mutex()
    {
    }
    void lock()
    {
        boost::mutex::lock();
    }
    void unlock()
    {
        boost::mutex::unlock();
    }
    bool tryLock()
    {
        return try_lock();
    }
};


}
}

#endif // MUTEX_HPP
