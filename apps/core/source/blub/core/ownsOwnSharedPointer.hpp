#ifndef OWNSOWNSHAREDPOINTER_HPP
#define OWNSOWNSHAREDPOINTER_HPP

#include "blub/core/globals.hpp"
#include "blub/core/noncopyable.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/enableSharedFromThis.hpp"


namespace blub
{

template<class classType>
class ownsOwnSharedPointer : public enableSharedFromThis<classType>, noncopyable
{
public:
    typedef enableSharedFromThis<classType> t_base;
    typedef blub::sharedPointer<classType> t_thisPtr;

    ownsOwnSharedPointer()
        : m_initialised(false)
    {
        ;
    }


    bool initialise()
    {
        m_initialised = true;

        m_sigInitalised(getThisPtr());

        return true;
    }

    void destroy()
    {
        BASSERT(m_initialised);
        m_initialised = false;

        m_sigDestroyed(getThisPtr());
        m_sigDestroyed.disconnectAllSlots();
    }

    t_thisPtr getThisPtr(void)
    {
        return t_thisPtr(t_base::shared_from_this());
    }

    typedef blub::signal<void (t_thisPtr about)> t_sigInitialised;
    t_sigInitialised *signalInitialised(void)
    {
        return &m_sigInitalised;
    }

    typedef blub::signal<void (t_thisPtr about)> t_sigDestroyed;
    t_sigDestroyed *signalDestroyed(void)
    {
        return &m_sigDestroyed;
    }

    const bool& isInitialised(void) const
    {
        return m_initialised;
    }


private:
    bool m_initialised;

    t_sigInitialised m_sigInitalised;
    t_sigDestroyed m_sigDestroyed;
};


}

#endif // OWNSOWNSHAREDPOINTER_HPP
