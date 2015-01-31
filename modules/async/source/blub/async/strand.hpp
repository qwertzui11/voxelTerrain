#ifndef ASYNC_STRAND_HPP
#define ASYNC_STRAND_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"

#include <boost/asio/strand.hpp>


namespace blub
{
namespace async
{


class strand
{
public:
    strand(dispatcher& disp);

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

    bool isRunningInThisThread() const
    {
        return m_service.running_in_this_thread();
    }

    const dispatcher &getDispatcher() const;

private:
    dispatcher &m_dispatcher;

    boost::asio::strand m_service;

};


}
}


#endif // ASYNC_STRAND_HPP
