#ifndef DEADLINETIMER_HPP
#define DEADLINETIMER_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"

#include <boost/asio/deadline_timer.hpp>


namespace blub
{
namespace async
{


class deadlineTimer
{
public:
    deadlineTimer(dispatcher* listener);

    template<typename CompletionHandler>
    void addToDoOnTimeoutMilli(CompletionHandler handler, const uint32& milli)
    {
        BASSERT(m_listener != nullptr);
        m_timer.expires_from_now(boost::posix_time::millisec(milli));
        m_timer.async_wait(handler);
    };

protected:
    dispatcher* m_listener;
    boost::asio::deadline_timer m_timer;

};


}
}


#endif // DEADLINETIMER_HPP
