#include "timer.hpp"

#include "blub/core/log.hpp"

#include <boost/chrono.hpp>


using namespace blub;

timer::timer(const string &outputText)
    : m_outputText(outputText)
{
}

void timer::start()
{
    m_timePoint = boost::chrono::system_clock::now();
}

real timer::end()
{
    boost::chrono::duration<real> sec = boost::chrono::system_clock::now() - m_timePoint;
    return sec.count();
}

real timer::endAndLog()
{
    real delta(end());
    BOUT(string(m_outputText) + " timer::delta:" + string::number(delta));
    return delta;
}
