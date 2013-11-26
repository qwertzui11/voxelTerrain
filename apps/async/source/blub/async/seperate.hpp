#ifndef ASYNC_SEPERATE_HPP
#define ASYNC_SEPERATE_HPP

#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"

#include <boost/asio/strand.hpp>


namespace blub
{
namespace async
{


class seperate
{
public:
    seperate(dispatcher& disp);

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

    const dispatcher &getDispatcher() const;

private:
    dispatcher &m_dispatcher;

    boost::asio::strand m_service;

};


}
}


#endif // ASYNC_SEPERATE_HPP
