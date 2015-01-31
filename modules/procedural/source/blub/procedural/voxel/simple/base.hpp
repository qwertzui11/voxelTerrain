#ifndef PROCEDURAL_VOXEL_SIMPLE_BASE_HPP
#define PROCEDURAL_VOXEL_SIMPLE_BASE_HPP

#include "blub/async/mutexReadWrite.hpp"
#include "blub/async/predecl.hpp"
#include "blub/core/bind.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/signal.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/async/strand.hpp"
#include "blub/math/vector3int.hpp"
#include "blub/procedural/log/global.hpp"
#include "blub/procedural/predecl.hpp"

#include <functional>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{


/**
 * @brief The base class gets derived by every class in the namespace simple::*.
 * It represends one level of detail.
 * If you dont need a terrain but a voxel-surface use a class in this namespace.
 */
template <class tileType>
class base : public noncopyable
{
public:
    typedef tileType t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;

    typedef base<t_tile> t_thisClass;

    /** id Identifier. Contains voxel from id*blub::procedural::voxel::tile::container::voxelLength to (id+1)*blub::procedural::voxel::tile::container::voxelLength-1 */
    typedef vector3int32 t_tileId;
    typedef hashMap<t_tileId, t_tilePtr> t_tilesGotChangedMap;

    typedef std::function<t_tilePtr ()> t_createTileCallback;

    /**
     * @brief base constructor
     * @param worker may gets run by multiple threads.
     */
    base(blub::async::dispatcher &worker);
    /**
     * @brief ~base destructor
     */
    ~base();

    /**
     * @brief lockForEdit locks the class for editing/writing it.
     * Call unlockForEdit() after work done.
     * Method executes locks by the dispatcher master.
     * @see getMaster()
     */
    void lockForEdit();
    /**
     * @brief unlockForEdit unlocks edit/write-lock and calls signalEditDone() .
     * Method executes unlock by the dispatcher master.
     * @see getMaster()
     */
    void unlockForEdit();
    /**
     * @brief lockForRead locks the class for reading.
     */
    void lockForRead();
    /**
     * @brief unlockRead unlocks the class after reading.
     */
    void unlockRead();

    /**
     * @brief getTilesThatGotEdited returns a list of tiles which changed since the last call lockForEdit() / lockForEditMaster()
     * @return
     */
    const t_tilesGotChangedMap &getTilesThatGotEdited() const;

    /**
     * @brief setCreateTileCallback sets a callback for creating tiles.
     * Use this method if you want to create custom tiles.
     * @param callback Must not be empty.
     */
    void setCreateTileCallback(const t_createTileCallback &callback);

    /**
     * @brief getMaster returns the master dispatcher.
     * The master synchronises jobs for the worker-thread and writes to class member.
     * The master calls all methods which end with *Master.
     * @return
     */
    blub::async::strand &getMaster();

    typedef blub::signal<void ()> t_sigEditDone;
    /**
     * @brief signalEditDone gets called after unlockForEdit() got called.
     * @return
     */
    t_sigEditDone* signalEditDone();

protected:
    /**
     * @brief addToChangeList adds a tile to the change-list.
     * @param id
     * @param toAdd
     * @see getTilesThatGotEdited()
     */
    void addToChangeList(const t_tileId& id, t_tilePtr toAdd);

    /**
     * @brief tryLockForEditMaster tries to lock for write. Call by master dispatcher.
     * @return True on successfully-lock.
     */
    virtual bool tryLockForEditMaster();
    /**
     * @brief lockForEditMaster locks for write, or waits until possible. Call by master dispatcher.
     */
    virtual void lockForEditMaster();
    /**
     * @brief unlockForEditMaster unlocks write. Call by master dispatcher.
     */
    virtual void unlockForEditMaster();

    /**
     * @brief createTile creates a new Tile. Uses callback set by setCreateTileCallback()
     * @return
     */
    virtual t_tilePtr createTile() const;

protected:
    /**
     * @brief m_master The master synchronises jobs for the worker-thread and writes to class member.
     * The master calls all methods which end with *Master.
     */
    blub::async::strand m_master;
    /**
     * @brief m_worker use it to dispatch heavy work. Don't write to class member with it.
     * Do not use any locks. Use m_master for such tasks.
     */
    blub::async::dispatcher &m_worker;

    t_tilesGotChangedMap m_tilesThatGotEdited;

    t_createTileCallback m_createTileCallback;

    async::mutexReadWrite m_classLocker;

    t_sigEditDone m_sigEditDone;
};

template <class tileType>
base<tileType>::base(async::dispatcher &worker)
    : m_master(worker)
    , m_worker(worker)
//    , m_createTileCallback(blub::bind(&t_tile::create)) // TODO good idea, techn difficult, via config
{
    ;
}

template <class tileType>
base<tileType>::~base()
{
}

template <class tileType>
void base<tileType>::lockForEdit()
{
    m_master.post(boost::bind(&base::lockForEditMaster, this));
}

template <class tileType>
void base<tileType>::unlockForEdit()
{
    m_master.post(boost::bind(&base::unlockForEditMaster, this));
}

template <class tileType>
void base<tileType>::lockForRead()
{
    m_classLocker.lockForRead();
}

template <class tileType>
void base<tileType>::unlockRead()
{
    m_classLocker.unlockRead();
}

template <class tileType>
const typename base<tileType>::t_tilesGotChangedMap &base<tileType>::getTilesThatGotEdited() const
{
    return m_tilesThatGotEdited;
}

template <class tileType>
void base<tileType>::setCreateTileCallback(const t_createTileCallback &callback)
{
    m_createTileCallback = callback;
}

template <class tileType>
void base<tileType>::addToChangeList(const t_tileId &id, t_tilePtr toAdd)
{
    BASSERT(!m_classLocker.tryLockForWrite());
    m_tilesThatGotEdited.insert(id, toAdd);
}

template <class tileType>
bool base<tileType>::tryLockForEditMaster()
{
    const bool result(m_classLocker.tryLockForWrite());
    if (result)
    {
        m_tilesThatGotEdited.clear();
    }
    return result;
}

template <class tileType>
void base<tileType>::lockForEditMaster()
{
    m_classLocker.lockForWrite();

    m_tilesThatGotEdited.clear();
}

template <class tileType>
void base<tileType>::unlockForEditMaster()
{
    m_classLocker.unlock();
#ifdef BLUB_LOG_VOXEL
    BLUB_PROCEDURAL_LOG_OUT() << "simple master unlock m_tilesThatGotEdited.size():" << m_tilesThatGotEdited.size();
#endif
    if (!m_tilesThatGotEdited.empty())
    {
        m_sigEditDone();
    }
}

template <class tileType>
typename base<tileType>::t_tilePtr base<tileType>::createTile() const
{
    return m_createTileCallback();
}

template <class tileType>
blub::async::strand &base<tileType>::getMaster()
{
    return m_master;
}

template <class tileType>
typename base<tileType>::t_sigEditDone *base<tileType>::signalEditDone()
{
    return &m_sigEditDone;
}

}
}
}
}


#endif // PROCEDURAL_VOXEL_SIMPLE_BASE_HPP
