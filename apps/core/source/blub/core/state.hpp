#ifndef STATE_HPP
#define STATE_HPP

#include <blub/core/signal.hpp>

namespace blub
{
    class state
    {
    public:
        state() {;}
        virtual ~state() {clean();}

        typedef blub::signal<void (state* st)> t_signalDone;
        t_signalDone *signalDone(void)
        {return &m_sigDone;}

        virtual bool initialise() {return true;}
        virtual void clean() {;}

        void callDone() {m_sigDone(this);}
    protected:

    private:
        t_signalDone m_sigDone;
    };
}

#endif // STATE_HPP
