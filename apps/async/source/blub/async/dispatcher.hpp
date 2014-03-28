#ifndef TODOLISTENER_HPP
#define TODOLISTENER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/list.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/string.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>


namespace boost
{
    class thread;
}


namespace blub
{
namespace async
{


class dispatcher : public noncopyable
{
public:
    dispatcher(const uint16& numThreads, const bool& endThreadsAfterAllDone, const string& threadName = "");
    virtual ~dispatcher();

    void join();
    void start();
    void run();
    void reset();
    void stop();

    template<typename CompletionHandler>
    void dispatch(CompletionHandler handler)
    {
        m_service.dispatch(handler);
    }

    template<typename CompletionHandler>
    void post(CompletionHandler handler)
    {
        m_service.post(handler);
    }

    /**
     * @brief waitForQueueDone will work only if one thread
     */
    void waitForQueueDone(void);

    int32 getThreadCount(void);

    boost::asio::io_service* _getIoService(void);

protected:
    friend class deadlineTimer;
    // network
    friend class network::ip::tcp::acceptor;
    friend class network::ip::tcp::connector;
    friend class network::ip::udp::socket;
    friend class network::connection;

    void nameThread(const int32 &indThread);

private:
    void runThread(const int32& indThread);

protected:
    const string m_threadName;

private:
    boost::asio::io_service m_service;
    boost::asio::io_service::work* m_work;
    uint16 m_numThreads;
    bool m_endThreadsAfterAllDone;
    typedef list<boost::thread*> t_threads;
    t_threads m_threads;

};


}
}


#endif // TODOLISTENER_HPP
