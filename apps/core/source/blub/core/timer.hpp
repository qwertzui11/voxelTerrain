#ifndef TIMER_HPP
#define TIMER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/string.hpp"

#include <boost/chrono/system_clocks.hpp>


namespace blub
{

class timer
{
public:
    timer(const string& outputText = blub::string("unnamed timer"));

    /**
     * @brief start gets a timepoint, call end to get the time passed from start()
     */
    void start();
    /**
     * @brief returns time in seconds, since last start() call
     */
    real end();

private:
    boost::chrono::system_clock::time_point m_timePoint;

    blub::string m_outputText;
};

}

#endif // TIMER_HPP
