#ifndef MUTEXREADWRITE_HPP
#define MUTEXREADWRITE_HPP

#include "blub/async/predecl.hpp"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>


namespace blub
{
namespace async
{

class mutexReadWrite : public boost::shared_mutex
{
public:
    typedef boost::shared_mutex t_base;

    mutexReadWrite()
    {
        ;
    }

    void lock()
    {
        t_base::lock();
    }
    void lockForRead()
    {
        t_base::lock_shared();
    }
    void unlockRead()
    {
        t_base::unlock_shared();
    }
    void lockForWrite()
    {
        t_base::lock();
    }
    bool tryLockForWrite()
    {
        return t_base::try_lock();
    }


};


typedef boost::unique_lock<boost::shared_mutex> mutexLockerWrite; // TODO: put into extra files; and take include with it
typedef boost::shared_lock<boost::shared_mutex> mutexLockerRead;


}
}

#endif // MUTEXREADWRITE_HPP
