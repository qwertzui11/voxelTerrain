#include "strand.hpp"

#include "blub/async/dispatcher.hpp"


using namespace blub::async;
using namespace blub;


strand::strand(dispatcher& disp)
    : m_dispatcher(disp)
    , m_service(*disp._getIoService())
{
}


const dispatcher &strand::getDispatcher() const
{
    return m_dispatcher;
}
