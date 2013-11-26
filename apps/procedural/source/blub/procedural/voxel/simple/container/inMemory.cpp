#include "inMemory.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/core/log.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/math/transform.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/edit/base.hpp"


using namespace blub::procedural::voxel::simple::container;
using namespace blub::procedural::voxel;


inMemory::inMemory(blub::async::dispatcher &worker, const blub::real &voxelSize)
    : t_base(worker, voxelSize)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("inMemory::inMemory()");
#endif
}

inMemory::~inMemory()
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("inMemory::~inMemory()");
#endif
}


utils::tile inMemory::getTileHolder(const blub::vector3int32 &id) const
{
    const axisAlignedBoxInt32 bounds(m_tiles.getBounds().getMinimum(), m_tiles.getBounds().getMaximum() - vector3int32(1));
    if (bounds.isInside(id))
    {
        return m_tiles.getValue(id);
    }
    return utils::tile();
}

void inMemory::setTileBounds(const blub::axisAlignedBoxInt32 &bounds)
{
    m_tiles.resize(bounds);
}

const blub::axisAlignedBoxInt32 &inMemory::getTileBounds() const
{
    return m_tiles.getBounds();
}

const inMemory::t_tilesMap &inMemory::getTilesMap() const
{
    return m_tiles;
}

void inMemory::setTileMaster(const blub::vector3int32 &id, const utils::tile &toSet)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("inMemory::setTileMaster id:" + blub::string::number(id));
#endif

    utils::tile holder(getTileHolder(id));
    bool alreadyEmpty(holder.state == utils::tileState::empty);

    if (toSet.state == utils::tileState::empty && alreadyEmpty)
    {
        return;
    }
    if (!alreadyEmpty)
    {
        if (toSet.state == utils::tileState::full &&
            holder.state == utils::tileState::full) // nothing will change
        {
            return;
        }
    }
    m_tiles.extend(axisAlignedBoxInt32(id, id+vector3int32(1)));
    m_tiles.setValue(id, toSet);

    if (toSet.state == utils::tileState::partitial)
    {
        BASSERT(!toSet.data->isEmpty());
        BASSERT(!toSet.data->isFull());

        if (!toSet.data->getEditedVoxelBoundingBox().isValid()) // no voxel changed
        {
            toSet.data->endEdit();
            return;
        }
    }
    t_base::addToChangeList(id, toSet);
}

void inMemory::setTileToFullMaster(const blub::vector3int32 &id)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("inMemory::setTileToFullMaster id:" + blub::string::number(id));
#endif
    BASSERT(!t_base::tryLockForEditMaster());

    const utils::tile holder(utils::tileState::full);
    setTileMaster(id, holder);
}

void inMemory::setTileToEmtpyMaster(const blub::vector3int32 &id)
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("inMemory::setTileToEmtpyMaster id:" + blub::string::number(id));
#endif
    BASSERT(!t_base::tryLockForEditMaster());

    const utils::tile holder(utils::tileState::empty);
    setTileMaster(id, holder);
}

