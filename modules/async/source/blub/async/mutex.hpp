#ifndef BLUB_ASYNC_MUTEX_HPP
#define BLUB_ASYNC_MUTEX_HPP

#include <mutex>


namespace blub
{
namespace async
{


class mutex : public std::mutex
{
public:
    mutex()
    {
    }
    bool tryLock()
    {
        return try_lock();
    }
};


}
}

#endif // BLUB_ASYNC_MUTEX_HPP
