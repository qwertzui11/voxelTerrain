#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "blub/core/globals.hpp"

#include <boost/signals2/signal.hpp>

namespace blub
{

template <typename T>
class signal : public boost::signals2::signal<T>
{
private:
    typedef boost::signals2::signal<T> t_base;

public:

    signal()
        : boost::signals2::signal<T>()
    {;}


    uint32 getNumSlots() const
    {
        return t_base::num_slots();
    }

    void disconnectAllSlots()
    {
        t_base::disconnect_all_slots();
    }
};

}


#endif // SIGNAL_HPP
