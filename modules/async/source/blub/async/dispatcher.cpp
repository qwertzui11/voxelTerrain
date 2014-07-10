#include "dispatcher.hpp"

#include "blub/async/mutex.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#ifdef BLUB_LINUX
#	include <sys/prctl.h>
#endif


using namespace blub::async;
using namespace blub;


dispatcher::dispatcher(const uint16 &numThreads, const bool &endThreadsAfterAllDone, const string &threadName)
    : m_threadName(threadName)
    , m_work(nullptr)
    , m_numThreads(numThreads)
    , m_endThreadsAfterAllDone(endThreadsAfterAllDone)
{
    m_service.reset(new boost::asio::io_service());
}


dispatcher::~dispatcher()
{
    stop();
}

void dispatcher::join()
{
    for (t_threads::iterator it = m_threads.begin(); it != m_threads.end(); ++it)
    {
        (*it)->join();
    }
}

void dispatcher::start()
{
    if (!m_endThreadsAfterAllDone)
    {
        m_work = new boost::asio::io_service::work(*m_service.get());
    }
    for (uint16 ind = 0; ind < m_numThreads; ++ind)
    {
        boost::thread *newOne(new boost::thread(boost::bind(&dispatcher::runThread, this, ind)));

        m_threads.push_back(newOne);
    }
    if (m_numThreads == 0)
    {
        run();
        m_service->reset();
    }
}

void dispatcher::run()
{
    m_service->run();
}

void dispatcher::reset()
{
    m_service->reset();
}

void dispatcher::stop()
{
    if (m_work != nullptr)
    {
        delete static_cast<boost::asio::io_service::work*>(m_work);
        m_work = nullptr;
    }
    // m_toDo.stop();
    join();

    m_threads.clear();
}

void dispatcher::dispatch(const dispatcher::t_toCallFunction &handler)
{
    m_service->dispatch(handler);
}

void dispatcher::post(const dispatcher::t_toCallFunction &handler)
{
    m_service->post(handler);
}

void dispatcher::waitForQueueDone()
{
    blub::async::mutex mutex;
    mutex.lock();
    post(boost::bind(&blub::async::mutex::unlock, &mutex));
    mutex.lock();
}

int32 dispatcher::getThreadCount()
{
    return m_threads.size();
}

boost::asio::io_service *dispatcher::_getIoService()
{
    return m_service.get();
}

void dispatcher::runThread(const int32& indThread)
{
    nameThread(indThread);
    run();
}

void dispatcher::nameThread(const int32& indThread)
{
#ifdef BLUB_LINUX
    blub::string threadName(m_threadName);
    if (m_numThreads > 1)
    {
        threadName += "_" + string::number(indThread);
    }
    if (m_threadName != "")
    {
        prctl(PR_SET_NAME, threadName.data(), 0, 0, 0);
    }
#else
    (void)indThread;
#endif
}

