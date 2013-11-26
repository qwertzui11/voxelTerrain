#ifndef FRAMELISTENER_HPP
#define FRAMELISTENER_HPP

#include "blub/core/globals.hpp"
#include <blub/core/signal.hpp>
#include "blub/async/dispatcher.hpp"


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

        void run(const real &frames = 60.0, const bool &threaded = false);

    protected:
        bool runOneFrame(const real& delta);

    private:
        const blub::string m_threadName;

        t_sigFrameListener m_sigFrame;
        boost::asio::io_service m_service;
        boost::asio::io_service::work m_workToDo;
        boost::asio::io_service m_toDoAfter;
        boost::asio::io_service::work m_workToDoAfter;
        boost::thread *m_thread;
        real m_frames;
        bool m_stop;
    };


}
}

#endif // FRAMELISTENER_HPP
