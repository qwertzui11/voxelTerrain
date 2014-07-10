#include "deadlineTimer.hpp"

#include "blub/async/dispatcher.hpp"


using namespace blub::async;
using namespace blub;


deadlineTimer::deadlineTimer(dispatcher *listener)
    : m_listener(listener)
    , m_timer(*m_listener->_getIoService())
{
    ;
}
