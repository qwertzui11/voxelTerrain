#include "base.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/procedural/voxel/data.hpp"
#include "blub/procedural/voxel/edit/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"


using namespace blub::procedural::voxel::simple::container;
using namespace blub::procedural::voxel;
using namespace blub;


base::base(blub::async::dispatcher &worker, const real& voxelSize)
    : t_base(worker)
    , m_voxelSize(voxelSize)
    , m_numInTilesInTask(0)
{

}

void base::editVoxel(t_editPtr change, const blub::transform& trans)
{
    m_master.post(boost::bind(&base::editMaster, this, change, trans));
}

void base::editVoxel(sharedPointer<edit::base> change, const transform &trans)
{
    editVoxel(change.staticCast<const edit::base>(), trans);
}

void base::setTile(const blub::vector3int32 &id, const utils::tile &toSet)
{
    m_master.post(boost::bind(&base::setTileMaster, this, id, toSet));
}

utils::tile base::getTileHolderByVoxelPosition(const blub::vector3int32 &pos) const
{
    return getTileHolder(calculateVoxelPosToTileId(pos));
}

data base::getVoxel(const blub::vector3int32 &voxelPos) const
{
    utils::tile workTile(getTileHolderByVoxelPosition(voxelPos));

    if (workTile.state == utils::tileState::full)
    {
        return tile::container::voxelInterpolationMaximum;
    }
    if (workTile.state == utils::tileState::empty)
    {
        return tile::container::voxelInterpolationMinimum;
    }
    return workTile.data->getVoxel(calculateVoxelPosInTile(voxelPos));
}

const blub::real &base::getVoxelSize() const
{
    return m_voxelSize;
}


void base::editMaster(t_editPtr change, const blub::transform& trans)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("base::editVoxel");
#endif

    editTodo edit(change, trans);

    m_editsTodo.push_back(edit);

    doNextEditMaster();
}

void base::doNextEditMaster(const bool &alreadyLocked)
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
    t_editPtr change(edit.edit_);

    const blub::transform trans(edit.trans);
    const blub::axisAlignedBox aabb(change->getAxisAlignedBoundingBox(m_voxelSize, trans));
    const blub::axisAlignedBoxInt32 aabbScaled(aabb.getMinimum() / m_voxelSize, aabb.getMaximum() / m_voxelSize);
    blub::vector3int32 startEdit;
    blub::vector3int32 endEdit;

    calculateAffectetedTilesByAabb(aabbScaled, &startEdit, &endEdit);

    for (blub::int32 indX = startEdit.x; indX < endEdit.x; ++indX)
    {
        for (blub::int32 indY = startEdit.y; indY < endEdit.y; ++indY)
        {
            for (blub::int32 indZ = startEdit.z; indZ < endEdit.z; ++indZ)
            {
                const blub::vector3int32 id(indX, indY, indZ);
                const utils::tile workTile(getTileHolder(id));

                ++m_numInTilesInTask;
                m_worker.post(boost::bind(&base::editVoxelWorker, this, change, workTile, id, trans));
            }
        }
    }
}

void base::editVoxelWorker(t_editPtr change, const utils::tile& holder, const blub::vector3int32 &id, const blub::transform& trans)
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
        workTile = createTile(holder.state == utils::tileState::full);
    }

    if (!workTile->getEditing())
    {
        workTile->startEdit();
    }
    change->calculateVoxel(workTile.data(), id, m_voxelSize, trans);

    utils::tile result;
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
    m_master.post(boost::bind(&base::editVoxelDoneMaster, this, result, id));
}

void base::editVoxelDoneMaster(utils::tile tileHolder_, const blub::vector3int32 &id)
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
            unlockForEditMaster();
        }
        else
        {
            doNextEditMaster(true);
        }
    }
}

base::t_tilePtr base::getOrCreateTile(const blub::vector3int32 &id)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("base::getOrCreateTile id:" + blub::string::number(id));
#endif

    utils::tile foundTile(getTileHolder(id));
    if (foundTile.state == utils::tileState::empty)
    {
        return createTile(false);
    }
    if (foundTile.state == utils::tileState::full)
    {
        return createTile(true);
    }
    return foundTile.data;
}

base::t_tilePtr base::createTile(const bool &full)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("base::createTile id: full:" + blub::string::number(full));
#endif
    t_tilePtr newOne(tile::container::create());

    if (full)
    {
        newOne->startEdit();
        newOne->setFull();
        newOne->endEdit();
    }

    return newOne;
}

void base::calculateAffectetedTilesByAabb(const axisAlignedBoxInt32& voxelAabb, vector3int32* startResult, vector3int32* endResult)
{
    blub::vector3 aabbScaledForTilesMin(voxelAabb.getMinimum());
    blub::vector3 aabbScaledForTilesMax(voxelAabb.getMaximum());

    *startResult = calculateVoxelPosToTileId(voxelAabb.getMinimum());
    *endResult = calculateVoxelPosToTileId(voxelAabb.getMaximum()) + blub::vector3int32(1, 1, 1);
}


blub::vector3int32 base::calculateVoxelPosToTileId(const blub::vector3int32 &voxelPos)
{
    return vector3int32((blub::vector3(voxelPos) / (real)tile::container::voxelLength).getFloor());
}

blub::vector3int32 base::calculateVoxelPosInTile(const blub::vector3int32 &voxelPos)
{
    vector3int32 result(voxelPos % tile::container::voxelLength);
    if (voxelPos.x < 0 && voxelPos.x%tile::container::voxelLength != 0)
    {
        result.x = tile::container::voxelLength + result.x;
    }
    if (voxelPos.y < 0 && voxelPos.y%tile::container::voxelLength != 0)
    {
        result.y = tile::container::voxelLength + result.y;
    }
    if (voxelPos.z < 0 && voxelPos.z%tile::container::voxelLength != 0)
    {
        result.z = tile::container::voxelLength + result.z;
    }
    return result;
}

void base::setVoxelSize(const blub::real &voxelSize)
{
    m_voxelSize = voxelSize;
}
