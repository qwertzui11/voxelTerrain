#ifndef PROCEDURAL_VOXEL_SIMPLE_BASE_HPP
#define PROCEDURAL_VOXEL_SIMPLE_BASE_HPP

#include "blub/async/mutexReadWrite.hpp"
#include "blub/async/predecl.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/signal.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/async/seperate.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/predecl.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


template <class dataType>
class base : public noncopyable
{
public:
    typedef dataType t_data;
    typedef base<t_data> t_thisClass;

    typedef vector3int32 t_tileId;
    typedef hashMap<t_tileId, t_data> t_tilesGotChangedMap;

    base(blub::async::dispatcher &worker);
    virtual ~base();

    void lockForEdit();
    void unlockForEdit();
    void lockForRead();
    void unlockRead();

    const t_tilesGotChangedMap &getTilesThatGotEdited(void) const;

    typedef blub::signal<void ()> t_sigEditDone;
    t_sigEditDone* signalEditDone(void);

protected:
    void addToChangeList(const t_tileId& id, t_data toAdd);

    virtual bool tryLockForEditMaster();
    virtual void lockForEditMaster();
    virtual void unlockForEditMaster();

protected:
    blub::async::seperate m_master;
    blub::async::dispatcher &m_worker;

private:
    t_tilesGotChangedMap m_tilesThatGotEdited;

    async::mutexReadWrite m_classLocker;

    t_sigEditDone m_sigEditDone;
};

template <class dataType>
base<dataType>::base(async::dispatcher &worker)
    : m_master(worker)
    , m_worker(worker)
{

}

template <class dataType>
base<dataType>::~base()
{
}

template <class dataType>
void base<dataType>::lockForEdit()
{
    m_master.post(boost::bind(&base::lockForEditMaster, this));
}

template <class dataType>
void base<dataType>::unlockForEdit()
{
    m_master.post(boost::bind(&base::unlockForEditMaster, this));
}

template <class dataType>
void base<dataType>::lockForRead()
{
    m_classLocker.lockForRead();
}

template <class dataType>
void base<dataType>::unlockRead()
{
    m_classLocker.unlockRead();
}

template <class dataType>
const typename base<dataType>::t_tilesGotChangedMap &base<dataType>::getTilesThatGotEdited() const
{
    return m_tilesThatGotEdited;
}

template <class dataType>
void base<dataType>::addToChangeList(const base::t_tileId &id, base::t_data toAdd)
{
    BASSERT(!m_classLocker.tryLockForWrite());
    m_tilesThatGotEdited.insert(id, toAdd);
}

template <class dataType>
bool base<dataType>::base::tryLockForEditMaster()
{
    return m_classLocker.tryLockForWrite();
    /*const bool result(t_base::tryLockForWrite()); // TODO: compiler (gcc-4.7.2) crashes - wait for new version and fix
    if (result)
    {
        m_tilesThatGotEdited.clear();
    }
    return result;*/
}

template <class dataType>
void base<dataType>::lockForEditMaster()
{
    m_classLocker.lockForWrite();

    m_tilesThatGotEdited.clear();
}

template <class dataType>
void base<dataType>::unlockForEditMaster()
{
    m_classLocker.unlock();
    if (!m_tilesThatGotEdited.empty())
    {
        m_sigEditDone();
    }
}

template <class dataType>
typename base<dataType>::t_sigEditDone *base<dataType>::signalEditDone()
{
    return &m_sigEditDone;
}

}
}
}
}


#endif // PROCEDURAL_VOXEL_SIMPLE_BASE_HPP
