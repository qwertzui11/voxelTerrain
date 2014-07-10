#ifndef BLUB_ASYNC_UPDATER_HPP
#define BLUB_ASYNC_UPDATER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/signal.hpp"
#include "blub/async/dispatcher.hpp"

#include <boost/chrono/system_clocks.hpp>


namespace blub
{
namespace async
{


    class updater : public dispatcher
    {
    public:
        updater(const blub::string& threadName);
        virtual ~updater();

        void stop();

        typedef blub::signal<void (real)> t_sigFrameListener;
        t_sigFrameListener* signalFrame();

        void run(const real &frames = 60.0, const bool &threaded = false); // change from frames to millis-wait

    protected:
        bool runOneFrame(const real& delta);
        void frame();

    private:
        const blub::string m_threadName;

        t_sigFrameListener m_sigFrame;

        boost::thread *m_thread;
        deadlineTimer *m_frameDeadline;

        boost::chrono::system_clock::time_point m_start;

        real m_frames;
        bool m_stop;
    };


}
}


#endif // BLUB_ASYNC_UPDATER_HPP
