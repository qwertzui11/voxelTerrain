#ifndef BLUB_CORE_DISPATCHER_HPP
#define BLUB_CORE_DISPATCHER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/list.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/scopedPtr.hpp"
#include "blub/core/string.hpp"

#include <functional>


namespace boost
{
    class thread;
    namespace asio
    {
        class io_service;
    }
}


namespace blub
{
namespace async
{


class dispatcher : public noncopyable // TODO free me from threads!
{
public:
    typedef std::function<void ()> t_toCallFunction;

    dispatcher(const uint16& numThreads = 0, const bool& endThreadsAfterAllDone = true, const string& threadName = "");
    virtual ~dispatcher();

    void join();
    void start();
    void run();
    void reset();
    void stop();

    void dispatch(const t_toCallFunction &handler);
    void post(const t_toCallFunction &handler);

    /**
     * @brief waitForQueueDone will work only if one thread
     */
    void waitForQueueDone(void);

    int32 getThreadCount(void);

    boost::asio::io_service* _getIoService(void);

protected:
    void nameThread(const int32 &indThread);

private:
    void runThread(const int32& indThread);

protected:
    const string m_threadName;

private:
    scopedPointer<boost::asio::io_service> m_service;
    void* m_work;
    uint16 m_numThreads;
    bool m_endThreadsAfterAllDone;
    typedef list<boost::thread*> t_threads;
    t_threads m_threads;

};


}
}


#endif // BLUB_CORE_DISPATCHER_HPP
