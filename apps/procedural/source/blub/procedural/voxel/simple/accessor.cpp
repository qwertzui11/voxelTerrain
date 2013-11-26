#include "accessor.hpp"

#include "blub/core/hashList.hpp"
#include "blub/core/log.hpp"
#include "blub/math/math.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"


using namespace blub::procedural::voxel::simple;
using namespace blub::procedural::voxel;
using namespace blub;


accessor::accessor(async::dispatcher &worker, container::base &voxels, const int32 &lod)
    : t_base(worker)
    , m_voxels(voxels)
    , m_lod(lod)
    , m_voxelSkip(math::pow(2, m_lod))
    , m_numTilesInWork(0)
{
    m_connTilesGotChanged = m_voxels.signalEditDone()->connect(boost::bind(&accessor::tilesGotChanged, this));
}

accessor::~accessor()
{
#ifdef BLUB_LOG_VOXEL
    blub::BOUT("accessor::~accessor()");
#endif
}

void accessor::tilesGotChanged()
{
    m_voxels.lockForRead();
    m_master.post(boost::bind(&accessor::tilesGotChangedMaster, this));
}

container::base &accessor::getVoxelContainer() const
{
    return m_voxels;
}


void accessor::tilesGotChangedMaster()
{
    const auto& changedTiles(m_voxels.getTilesThatGotEdited());
    BASSERT(!changedTiles.empty());
    /*if (changedTiles.empty())
    {
        m_voxels.unlockRead();
        return;
    }*/

    t_tileIdList affectedTiles;
    for (auto change : changedTiles)
    {
        const t_tileId id(change.first);
        const t_tileHolder workTile(change.second);

        if (m_lod == 0) // optimisation on lod_0 only
        {
            if (workTile.state == container::utils::tileState::empty)
            {
                if (m_tiles.find(id) == m_tiles.cend())
                {
                    continue;
                }
            }
        }

        calculateAffectedAccessorTilesByContainerTile(id, workTile, affectedTiles);
    }

    if (affectedTiles.empty())
    {
        blub::BWARNING("affectedTiles.empty()");
        m_voxels.unlockRead();
        return;
    }

    t_base::lockForEditMaster();
    BASSERT(m_numTilesInWork == 0);
    m_numTilesInWork = affectedTiles.size();

    for (const t_tileId id : affectedTiles)
    {
        m_worker.post(boost::bind(&accessor::calculateAccessorTS, this, id, getTile(id)));
    }
}

void accessor::calculateAccessorTS(const accessor::t_tileId &id, t_tilePtr workTile)
{
    if (workTile.isNull())
    {
        workTile = createTile();
    }
    workTile->setEmpty();

    const vector3int32 voxelStart(id*tile::accessor::voxelLength*m_voxelSkip);
    axisAlignedBoxInt32 lastUsedTileBounds;
    t_tileHolder lastUsedTile;

    bool valuesChanged(false);
    for (int32 indX = -1; indX < tile::accessor::voxelLength+2; ++indX)
    {
        for (int32 indY = -1; indY < tile::accessor::voxelLength+2; ++indY)
        {
            for (int32 indZ = -1; indZ < tile::accessor::voxelLength+2; ++indZ)
            {
                const vector3int32 pos(indX, indY, indZ);
                const vector3int32 voxelPosAbs(voxelStart + pos*m_voxelSkip);

                valuesChanged |= workTile->setVoxel(pos, getVoxelData(voxelPosAbs, lastUsedTileBounds, lastUsedTile));
            }
        }
    }

    if (workTile->getCalculateLod())
    {
        BASSERT(m_voxelSkip > 0);

        const int32 voxelLengthLod = tile::accessor::voxelLengthLod;
        const vector3int32 toIterate[][2] = {
                                            {{0, 0, 0}, {1, voxelLengthLod, voxelLengthLod}},
                                            {{voxelLengthLod-2, 0, 0}, {voxelLengthLod-1, voxelLengthLod, voxelLengthLod}},
                                            {{0, 0, 0}, {voxelLengthLod, 1, voxelLengthLod}},
                                            {{0, voxelLengthLod-2, 0}, {voxelLengthLod, voxelLengthLod-1, voxelLengthLod}},
                                            {{0, 0, 0}, {voxelLengthLod, voxelLengthLod, 1}},
                                            {{0, 0, voxelLengthLod-2}, {voxelLengthLod, voxelLengthLod, voxelLengthLod-1}},
                                            };
        for (int32 lod = 0; lod < 6; ++lod)
        {
            const vector3int32& start(toIterate[lod][0]);
            const vector3int32& end(toIterate[lod][1]);
            for (int32 indX = start.x; indX < end.x; ++indX)
            {
                for (int32 indY = start.y; indY < end.y; ++indY)
                {
                    for (int32 indZ = start.z; indZ < end.z; ++indZ)
                    {
                        const vector3int32 pos(indX, indY, indZ);
                        const vector3int32 voxelPosAbs(voxelStart + pos*(m_voxelSkip/2));

                        const data result(getVoxelData(voxelPosAbs, lastUsedTileBounds, lastUsedTile));
#ifdef BLUB_DEBUG
                        if (indX % 2 == 0 &&
                            indY % 2 == 0 &&
                            indZ % 2 == 0)
                        {
                            BASSERT(workTile->getVoxel(pos / 2) == result);
                        }
#endif

                        valuesChanged |= workTile->setVoxelLod(pos-start, result, lod);
                    }
                }
            }
        }
    }

    if (workTile->isEmpty() || workTile->isFull())
    {
        m_master.post(boost::bind(&accessor::afterCalculateAccessorMaster, this, id, nullptr, true));
        return;
    }

    m_master.post(boost::bind(&accessor::afterCalculateAccessorMaster, this, id, workTile, valuesChanged));
}

void accessor::afterCalculateAccessorMaster(const accessor::t_tileId &id, accessor::t_tilePtr workTile, const bool& didValuesChanged)
{
    BASSERT(t_base::getTilesThatGotEdited().find(id) == t_base::getTilesThatGotEdited().cend());

    t_tiles::const_iterator it = m_tiles.find(id);

    // no indices
    if (workTile.isNull())
    {
        if (it != m_tiles.cend())
        {
            m_tiles.erase_return_void(it);
            addToChangeList(id, nullptr);
        }
    }
    else
    {
        BASSERT(!workTile.isNull());
        if (it == m_tiles.cend())
        {
            m_tiles.insert(id, workTile);
        }
        if (didValuesChanged)
        {
            addToChangeList(id, workTile);
        }
    }


    --m_numTilesInWork;
    if (m_numTilesInWork == 0)
    {
        m_voxels.unlockRead();
        unlockForEditMaster();
    }
}

void accessor::calculateAffectedAccessorTilesByContainerTile(const accessor::t_tileId &conterainerId, const accessor::t_tileHolder& holder, accessor::t_tileIdList &resultingSurfaceTiles)
{
    const vector3 forDiv(conterainerId);
    const vector3int32 result((forDiv / (real)m_voxelSkip).getFloor());
    resultingSurfaceTiles.insert(result);

    vector3int32 minimum(0);
    if (holder.state == t_tileState::partitial)
    {
        BASSERT(holder.data->getEditedVoxelBoundingBox().isValid());
        minimum = holder.data->getEditedVoxelBoundingBox().getMinimum();
    }
    if (minimum > vector3int32(0))
    {
        return;
    }

    const vector3int32 resultMod(conterainerId%m_voxelSkip);
    if (resultMod.x == 0 && minimum.x == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(1, 0, 0));
    }
    if (resultMod.y == 0 && minimum.y == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(0, 1, 0));
    }
    if (resultMod.z == 0 && minimum.z == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(0, 0, 1));
    }
    if (resultMod.x == 0 && resultMod.y == 0 && minimum.x == 0 && minimum.y == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(1, 1, 0));
    }
    if (resultMod.x == 0 && resultMod.z == 0 && minimum.x == 0 && minimum.z == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(1, 0, 1));
    }
    if (resultMod.y == 0 && resultMod.z == 0 && minimum.y == 0 && minimum.z == 0)
    {
        resultingSurfaceTiles.insert(result - vector3int32(0, 1, 1));
    }
    if (resultMod.x == 0 && resultMod.y == 0 && resultMod.z == 0 && minimum == vector3int32(0))
    {
        resultingSurfaceTiles.insert(result - vector3int32(1, 1, 1));
    }
}

accessor::t_tilePtr accessor::getTile(const accessor::t_tileId &id) const
{
    t_tiles::const_iterator it = m_tiles.find(id);
    if (it == m_tiles.cend())
    {
        return nullptr;
    }
    return it->second;
}

accessor::t_tilePtr accessor::createTile() const
{
    return tile::accessor::create(m_lod > 0);
}

data accessor::getVoxelData(const vector3int32 &voxelPosAbs, axisAlignedBoxInt32 &lastUsedTileBounds, accessor::t_tileHolder &lastUsedTile)
{
    if (lastUsedTileBounds.isInside(voxelPosAbs))
    {
        switch(lastUsedTile.state)
        {
        case t_tileState::partitial:
            return lastUsedTile.data->getVoxel(voxelPosAbs-lastUsedTileBounds.getMinimum()).interpolation;
        case t_tileState::empty:
            return tile::container::voxelInterpolationMinimum;
        case t_tileState::full:
            return tile::container::voxelInterpolationMaximum;
        default:
            BASSERT(false);
        }
        BASSERT(false);
    }

    // get new tile
    const vector3int32 tilePos(m_voxels.calculateVoxelPosToTileId(voxelPosAbs));
    lastUsedTile = m_voxels.getTileHolder(tilePos);
    const vector3int32 tilePosAbs(tilePos*vector3int32(tile::container::voxelLength));
    lastUsedTileBounds = axisAlignedBoxInt32(tilePosAbs, tilePosAbs + vector3int32(tile::container::voxelLength-1));

    // repeat step ahead
    BASSERT(lastUsedTileBounds.isInside(voxelPosAbs));
    switch(lastUsedTile.state)
    {
    case t_tileState::partitial:
        return lastUsedTile.data->getVoxel(voxelPosAbs-lastUsedTileBounds.getMinimum()).interpolation;
    case t_tileState::empty:
        return tile::container::voxelInterpolationMinimum;
    case t_tileState::full:
        return tile::container::voxelInterpolationMaximum;
    default:
        BASSERT(false);
    }
    BASSERT(false);
    return 0;
}
