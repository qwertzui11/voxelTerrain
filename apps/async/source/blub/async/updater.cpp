#include "updater.hpp"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#ifdef BLUB_LINUX
#include <sys/prctl.h>
#endif


using namespace blub::async;
using namespace blub;


updater::updater(const string &threadName)
    : dispatcher(0, true, threadName)
    , m_workToDo(m_service)
    , m_workToDoAfter(m_toDoAfter)
    , m_thread(nullptr)
    , m_stop(false)
{
}


updater::~updater()
{
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
    real delta = 0.01;
    while(!m_stop)
    {
        boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
        runOneFrame(1.0/m_frames);
        boost::chrono::duration<double> sec = boost::chrono::system_clock::now() - start;
        delta = sec.count();
        if (frames > 0.0)
        {
            if (delta < 1.0/frames)
            {
#ifdef BLUB_WIN
                Sleep( ((1.0/frames)-delta) *1000.0);
#else
                usleep( ((1.0/frames)-delta) *1000000.0);
#endif
            }
        }
    }
}

bool updater::runOneFrame(const real& delta)
{
    dispatcher::start();
    m_sigFrame(delta);
    return true;
}
