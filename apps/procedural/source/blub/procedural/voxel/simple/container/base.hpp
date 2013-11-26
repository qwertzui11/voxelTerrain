#ifndef VOXEL_SIMPLE_CONTAINER_BASE_HPP
#define VOXEL_SIMPLE_CONTAINER_BASE_HPP

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


class base : public simple::base<utils::tile>
{
public:
    typedef simple::base<utils::tile> t_base;

    typedef sharedPointer<edit::base const> t_editPtr;
    typedef sharedPointer<tile::container> t_tilePtr;
    typedef vector3int32 t_tileId;

    base(blub::async::dispatcher &worker, const real& voxelSize);

    void editVoxel(t_editPtr change, const transform &trans = blub::transform());
    void editVoxel(sharedPointer<edit::base> change, const transform &trans = blub::transform());
    void setTile(const blub::vector3int32& id, const utils::tile& toSet);
    /**
     * @brief getTileHolder lock class before call
     * @param id
     * @return
     */
    virtual utils::tile getTileHolder(const blub::vector3int32& id) const = 0;
    utils::tile getTileHolderByVoxelPosition(const blub::vector3int32& pos) const;

    const real& getVoxelSize() const;

    data getVoxel(const vector3int32& voxelPos) const;

    static vector3int32 calculateVoxelPosToTileId(const vector3int32& voxelPos);
    static vector3int32 calculateVoxelPosInTile(const vector3int32& voxelPos);

protected:
    void setVoxelSize(const real& voxelSize);

    struct editTodo
    {
        editTodo(const t_editPtr change, const blub::transform& trans_)
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

        t_editPtr edit_;
        transform trans;
    };


    void editMaster(t_editPtr change, const blub::transform& trans);
    void doNextEditMaster(const bool &alreadyLocked = false);
    void editVoxelWorker(t_editPtr change, const utils::tile &tileHolder, const blub::vector3int32& id, const blub::transform& trans);
    void editVoxelDoneMaster(utils::tile tileHolder, const blub::vector3int32& id);

    virtual void setTileMaster(const blub::vector3int32& id, const utils::tile& toSet) = 0;

    t_tilePtr getOrCreateTile(const vector3int32& id);
    static t_tilePtr createTile(const bool &full = false);
    virtual void setTileToFullMaster(const vector3int32& id) = 0;
    virtual void setTileToEmtpyMaster(const vector3int32& id) = 0;

    static void calculateAffectetedTilesByAabb(const axisAlignedBoxInt32& voxelAabb, vector3int32* startResult, vector3int32* endResult);

protected:
    real m_voxelSize;
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
