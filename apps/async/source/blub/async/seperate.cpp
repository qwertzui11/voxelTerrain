#include "seperate.hpp"

#include "blub/async/dispatcher.hpp"


using namespace blub::async;
using namespace blub;


seperate::seperate(dispatcher& disp)
    : m_dispatcher(disp)
    , m_service(*disp._getIoService())
{
}


const dispatcher &seperate::getDispatcher() const
{
    return m_dispatcher;
}
