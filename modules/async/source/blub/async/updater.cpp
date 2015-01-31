#include "updater.hpp"

#include "blub/async/deadlineTimer.hpp"
#include "blub/async/log/global.hpp"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#ifdef BLUB_LINUX
#include <sys/prctl.h>
#endif


using namespace blub::async;
using namespace blub;


updater::updater(const string &threadName)
    : dispatcher(0, true, threadName)
    , m_thread(nullptr)
    , m_frameDeadline(nullptr)
    , m_stop(false)
{
}


updater::~updater()
{
	BLUB_ASYNC_LOG_OUT() << "destructor m_threadName:" << m_threadName;
    stop();
}

void updater::stop()
{
    m_stop = true;
    if (m_thread != nullptr)
    {
        if (m_thread->joinable())
        {
            m_thread->join();
        }
        delete m_thread;
        m_thread = nullptr;
    }
}

updater::t_sigFrameListener *updater::signalFrame()
{
    return &m_sigFrame;
}

void updater::run(const real& frames, const bool& threaded)
{
    m_frames = frames;
    if (threaded)
    {
        BASSERT(m_thread == nullptr);
        m_thread = new boost::thread(boost::bind(&updater::run, this, frames, false));
        return;
    }
    nameThread(0);

    if (m_frames != 0.)
    {
        m_frameDeadline = new deadlineTimer(*this);
    }

    m_start = boost::chrono::system_clock::now();
    if (m_frameDeadline != nullptr)
    {
        uint32 millis(static_cast<uint32>((1./m_frames)*1000.));
        m_frameDeadline->addToDoOnTimeoutMilli(boost::bind(&updater::frame, this), millis);
    }
    else
    {
        post(boost::bind(&updater::frame, this));
    }
    dispatcher::start();

    BLUB_ASYNC_LOG_OUT() << "run end m_threadName:" << m_threadName;
}

bool updater::runOneFrame(const real& delta)
{
    dispatcher::start();
    m_sigFrame(delta);
    return true;
}

void updater::frame()
{
    boost::chrono::duration<double> delta = boost::chrono::system_clock::now() - m_start;
    m_start = boost::chrono::system_clock::now();
    if (m_frameDeadline != nullptr && !m_stop)
    {
        m_frameDeadline->addToDoOnTimeoutMilli(boost::bind(&updater::frame, this), static_cast<uint32>((1./m_frames)*1000.));
    }
    m_sigFrame(delta.count());
    if (m_frameDeadline == nullptr && !m_stop)
    {
        post(boost::bind(&updater::frame, this));
    }
}
