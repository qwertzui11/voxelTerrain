#ifndef VOXEL_SIMPLE_CONTAINER_BASE_HPP
#define VOXEL_SIMPLE_CONTAINER_BASE_HPP

#include "blub/core/sharedPointer.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/math/transform.hpp"
#include "blub/procedural/voxel/simple/base.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace simple
{
namespace container
{


/**
 * @brief The base class is getting derived by all container classes.
 * The class handles synchronisation and paralellisation of edits and provides several voxel-getter/setter.
 * All methods are threadsafe, if not declared different.
 */
template <class voxelType>
class base : public simple::base<utils::tile<voxelType> > // FIXME ugly as hell remove util::tile, make more template parameter, remove base<sharedPointer<tile>> in all other simple classes
{
public:
    typedef voxelType t_voxel;
    typedef utils::tile<voxelType> t_utilsTile;
    typedef simple::base<t_utilsTile> t_base;

    typedef sharedPointer<edit::base<voxelType> const> t_editConstPtr;
    typedef sharedPointer<edit::base<voxelType> > t_editPtr;
    typedef tile::container<voxelType> t_tile;
    typedef sharedPointer<t_tile> t_tilePtr;
    typedef typename t_base::t_tileId t_tileId;

    /**
     * @brief base constructor.
     * @param worker may gets run by several threads.
     */
    base(blub::async::dispatcher &worker)
        : t_base(worker)
        , m_numInTilesInTask(0)
    {

    }

    /**
     * @brief editVoxel edits the container.
     * Its guranteed that the edits are getting in order of calling this method.
     * Returns immediately. Does NOT calculate any voxel, but creates and dispatches the jobs for it.
     * @param change Must not be nullptr
     * @param trans The transform of the edit.
     */
    void editVoxel(t_editConstPtr change, const transform &trans = blub::transform())
    {
        t_base::m_master.post(boost::bind(&base::editMaster, this, change, trans));
    }

    /**
     * @brief editVoxel edits the container.
     * Its guranteed that the edits are getting in order of calling this method.
     * Returns immediately. Does NOT calculate any voxel, but creates and dispatches the jobs for it.
     * @param change Must not be nullptr
     * @param trans The transform of the edit.
     */
//    void editVoxel(t_editPtr change, const transform &trans = blub::transform())
//    {
//        editVoxel(change.template staticCast<const edit::base<voxelType> >(), trans);
//    }

    /**
     * @brief setTile sets a tile. You may wanna call this method on loading or synchronisation. Method is threadsafe. Write-lock class before.
     * @param id Identifier. Contains voxel from id*blub::procedural::voxel::tile::container::voxelLength to (id+1)*blub::procedural::voxel::tile::container::voxelLength-1
     * @param toSet Tile to set.
     * @see setTileMaster
     */
    void setTile(const t_tileId& id, const t_utilsTile& toSet)
    {
        t_base::m_master.post(boost::bind(&base::setTileMaster, this, id, toSet));
    }

    /**
     * @brief getTileHolder returns a utils::tileHolder setted by setTile() or by editVoxel(). Read-lock class before call.
     * Read-lock the class before.
     * @param id Identifier. Contains voxel from id*blub::procedural::voxel::tile::container::voxelLength to (id+1)*blub::procedural::voxel::tile::container::voxelLength-1
     * @return Always returns a valid value. If nothing found state in utils::tile gets set to empty.
     */
    virtual t_utilsTile getTileHolder(const t_tileId& id) const = 0;
    /**
     * @brief getTileHolderByVoxelPosition returns a utils::tileHolder setted by setTile() or by editVoxel(). Read-lock class before call.
     * @param pos An absolute voxel-coordinate.
     * @return Always returns a valid value. If nothing found state in utils::tile gets set to empty.
     */
    t_utilsTile getTileHolderByVoxelPosition(const blub::vector3int32& pos) const
    {
        return getTileHolder(calculateVoxelPosToTileId(pos));
    }

    /**
     * @brief getVoxel returns an voxel. Read-lock the class before call.
     * @param voxelPos An absolute voxel-coordinate.
     * @return always returns a valid value.
     */
    t_voxel getVoxel(const vector3int32& voxelPos) const
    {
        t_utilsTile workTile(getTileHolderByVoxelPosition(voxelPos));

        t_voxel result;
        if (workTile.state == utils::tileState::full)
        {
            result.setMax();
            return result;
        }
        if (workTile.state == utils::tileState::empty)
        {
            result.setMin();
            return result;
        }
        return workTile.data->getVoxel(calculateVoxelPosInTile(voxelPos));
    }

    /**
     * @brief calculateVoxelPosToTileId converts an absolute voxel-position to an relative container-id position.
     * @param voxelPos An absolute voxel-postion.
     * @return A absolute tile-id
     */
    static t_tileId calculateVoxelPosToTileId(const vector3int32& voxelPos)
    {
        return vector3int32((blub::vector3(voxelPos) / (real)tile::container<voxelType>::voxelLength).getFloor());
    }

    /**
     * @brief calculateVoxelPosInTile converts an absolute voxel-position to an relative container-confirm position.
     * @param voxelPos An absolute voxel-postion.
     * @return A inside tile position. Voxel-position of voxelPos inside a tile.
     */
    static vector3int32 calculateVoxelPosInTile(const vector3int32& voxelPos) // TODO floor?!
    {
        vector3int32 result(voxelPos % tile::container<voxelType>::voxelLength);
        if (voxelPos.x < 0 && voxelPos.x%tile::container<voxelType>::voxelLength != 0)
        {
            result.x = tile::container<voxelType>::voxelLength + result.x;
        }
        if (voxelPos.y < 0 && voxelPos.y%tile::container<voxelType>::voxelLength != 0)
        {
            result.y = tile::container<voxelType>::voxelLength + result.y;
        }
        if (voxelPos.z < 0 && voxelPos.z%tile::container<voxelType>::voxelLength != 0)
        {
            result.z = tile::container<voxelType>::voxelLength + result.z;
        }
        return result;
    }

protected:
    /**
     * @brief The editTodo struct holds the edit information, for the dispatcher. A primitive buffer for the paramters delivered by editVoxel().
     */
    struct editTodo
    {
        editTodo(const t_editConstPtr change, const blub::transform& trans_)
            : edit_(change)
            , trans(trans_)
        {
            ;
        }
        editTodo(const editTodo& other)
            : edit_(other.edit_)
            , trans(other.trans)
        {
            ;
        }
        const editTodo& operator = (const editTodo& other)
        {
            edit_ = other.edit_;
            trans = other.trans;

            return *this;
        }

        t_editConstPtr edit_;
        transform trans;
    };


    /**
     * @brief editMaster saves the edit in a qeue and starts to handle it.
     * @param change Must not be nullptr.
     * @param trans The transform.
     */
    void editMaster(t_editConstPtr change, const blub::transform& trans)
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("base::editVoxel");
    #endif

        editTodo edit(change, trans);

        m_editsTodo.push_back(edit);

        doNextEditMaster();
    }

    /**
     * @brief doNextEditMaster finds out which tiles the edit affects and dispaches the change to the worker-threads.
     * @param alreadyLocked optimization parameter, if class is already write locked. (indirect recursive calls)
     */
    void doNextEditMaster(const bool &alreadyLocked = false)
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("base::doNextEditMaster");
    #endif
        BASSERT(m_numInTilesInTask >= 0);

        if (m_editsTodo.empty())
        {
            return;
        }
        if (m_numInTilesInTask > 0)
        {
            return;
        }

        if (!alreadyLocked)
        {
            t_base::lockForEditMaster();
        }
        BASSERT(!t_base::tryLockForEditMaster());

        BASSERT(m_numInTilesInTask == 0);

        editTodo edit(*m_editsTodo.begin());
        m_editsTodo.erase(m_editsTodo.begin());
        t_editConstPtr change(edit.edit_);

        const blub::transform trans(edit.trans);
        const blub::axisAlignedBox aabb(change->getAxisAlignedBoundingBox(trans));
        const blub::axisAlignedBoxInt32 aabbScaled(aabb.getMinimum(), aabb.getMaximum());
        blub::vector3int32 startEdit;
        blub::vector3int32 endEdit;

        calculateAffectetedTilesByAabb(aabbScaled, startEdit, endEdit);

        for (blub::int32 indX = startEdit.x; indX < endEdit.x; ++indX)
        {
            for (blub::int32 indY = startEdit.y; indY < endEdit.y; ++indY)
            {
                for (blub::int32 indZ = startEdit.z; indZ < endEdit.z; ++indZ)
                {
                    const blub::vector3int32 id(indX, indY, indZ);
                    const t_utilsTile workTile(getTileHolder(id));

                    ++m_numInTilesInTask;
                    t_base::m_worker.post(boost::bind(&base::editVoxelWorker, this, change, workTile, id, trans));
                }
            }
        }
    }

    /**
     * @brief editVoxelWorker affects with change holder. Method gets called paralell by various threads.
     * Method only affects the delivered holder.
     * @param change The edit.
     * @param holder The tile which gets affected.
     * @param id TileId.
     * @param trans Transform.
     */
    void editVoxelWorker(t_editConstPtr change, const t_utilsTile &holder, const blub::vector3int32& id, const blub::transform& trans)
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("base::editVoxelTS id:" + blub::string::number(id));
    #endif
        t_tilePtr workTile;
        if (holder.state == utils::tileState::partitial)
        {
            workTile = holder.data;
        }
        else
        {
            workTile = createTileFull(holder.state == utils::tileState::full);
        }

        if (!workTile->getEditing())
        {
            workTile->startEdit();
        }
        change->calculateVoxel(workTile.data(), id, trans);

        t_utilsTile result;
        if (workTile->isEmpty())
        {
            result.state = utils::tileState::empty;
        }
        else
        {
            if (workTile->isFull())
            {
                result.state = utils::tileState::full;
            }
            else
            {
                result.state = utils::tileState::partitial;
                result.data = workTile;
            }
        }
        t_base::m_master.post(boost::bind(&base::editVoxelDoneMaster, this, result, id));
    }

    /**
     * @brief editVoxelDoneMaster gets called after edit got applied on the tile. Call method only by one thread at a time.
     * @param tileHolder_ Affected tile.
     * @param id Tile identifier.
     */
    void editVoxelDoneMaster(t_utilsTile tileHolder_, const blub::vector3int32& id)
    {
        setTileMaster(id, tileHolder_);

        --m_numInTilesInTask;
        if (m_numInTilesInTask == 0)
        {
            if (m_editsTodo.isEmpty())
            {
                // unlock all tiles
                for (auto work : t_base::getTilesThatGotEdited())
                {
                    if (!work.second.data.isNull())
                    {
                        work.second.data->endEdit();
                    }
                }
                t_base::unlockForEditMaster();
            }
            else
            {
                doNextEditMaster(true);
            }
        }
    }

    /**
     * @brief gets called by setTile. Call only by one thread at a time. Write-lock class before.
     * @see setTile
     */
    virtual void setTileMaster(const t_tileId& id, const t_utilsTile& toSet) = 0;

    /**
     * @brief getOrCreateTile looks up if tile already exists. If not creates one.
     * @param id TileId
     * @return Valid Tile. Never nullptr.
     */
    t_tilePtr getOrCreateTile(const vector3int32& id)
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("base::getOrCreateTile id:" + blub::string::number(id));
    #endif

        t_utilsTile foundTile(getTileHolder(id));
        if (foundTile.state == utils::tileState::empty)
        {
            return createTileFull(false);
        }
        if (foundTile.state == utils::tileState::full)
        {
            return createTileFull(true);
        }
        return foundTile.data;
    }

    /**
     * @brief createTileFull creates a tile in which all voxel are maximum or minimum.
     * @param full If false all voxel get initialised with default value.
     * @return Never nullptr.
     */
    static t_tilePtr createTileFull(const bool &full)
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("base::createTile id: full:" + blub::string::number(full));
    #endif
        t_tilePtr newOne(t_tile::create()); // FIXME call base class

        if (full)
        {
            newOne->startEdit();
            newOne->setFull();
            newOne->endEdit();
        }

        return newOne;
    }

    /**
     * @brief setTileToFullMaster sets a tiles voxel to maximum - for memory optimizations. Method gets called by a single thread at a time.
     * @param id TileId
     */
    virtual void setTileToFullMaster(const t_tileId& id) = 0;
    /**
     * @brief setTileToFullMaster sets a tiles voxel to minimum - for memory optimizations. Method gets called by a single thread at a time.
     * @param id TileId
     */
    virtual void setTileToEmtpyMaster(const t_tileId& id) = 0;

    /**
     * @brief calculateAffectetedTilesByAabb caluclates a list of affected tiles by an axisAlignedBox. Used to determine which tiles to recalculate for an edit.
     * @param voxelAabb axisAlignedBox
     * @param startResult Resulting minId.
     * @param endResult Resulting maxId.
     */
    static void calculateAffectetedTilesByAabb(const axisAlignedBoxInt32& voxelAabb, t_tileId& startResult, t_tileId& endResult)
    {
        startResult = calculateVoxelPosToTileId(voxelAabb.getMinimum());
        endResult = calculateVoxelPosToTileId(voxelAabb.getMaximum()) + blub::vector3int32(1, 1, 1);
    }

protected:
    int32 m_numInTilesInTask;

    typedef list<editTodo> t_editTodoList;
    t_editTodoList m_editsTodo;
};


}
}
}
}
}

#endif // VOXEL_SIMPLE_CONTAINER_BASE_HPP
