#include "surface.hpp"

#include "blub/core/log.hpp"
#include "blub/math/math.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"


using namespace blub::procedural::voxel::simple;
using namespace blub::procedural::voxel;
using namespace blub;


surface::surface(blub::async::dispatcher &worker,
                 t_voxelAccessor &voxels,
                 const int32& lod,
                 const real &voxelSize)
    : t_base(worker)
    , m_voxels(voxels)
    , m_tileSize(math::pow(2, lod))
    , m_lod(lod)
    , m_numTilesInWork(0)
    , m_voxelSize(voxelSize)
{
    voxels.signalEditDone()->connect(boost::bind(&surface::editDone, this));
}


surface::~surface()
{
    ;
}

int32 surface::getTileCount() const
{
    return m_tiles.size();
}

const real &surface::getVoxelSize() const
{
    return m_voxelSize;
}

surface::t_tilePtr surface::getTile(const vector3int32 &id) const
{
    t_tilesMap::const_iterator it(m_tiles.find(id));
    if (it == m_tiles.cend())
    {
        return nullptr;
    }
    return it->second;
}

surface::t_tilePtr surface::getOrCreateTile(const vector3int32 &id) const
{
    t_tilePtr workTile(getTile(id));
    if (workTile.isNull())
    {
        return createTile();
    }
    return workTile;
}

void surface::editDone()
{
    m_voxels.lockForRead();

    m_master.post(boost::bind(&surface::editDoneMaster, this));
}


surface::t_tilePtr surface::createTile()
{
    return t_tilePtr(tile::surface::create());
}

void surface::editDoneMaster()
{
    const accessor::t_tilesGotChangedMap& change(m_voxels.getTilesThatGotEdited());
    if (change.empty())
    {
        blub::BWARNING("change.empty()");
        return;
    }

    t_base::lockForEditMaster();

    BASSERT(m_numTilesInWork == 0);
    m_numTilesInWork = change.size();

    for (auto work : change)
    {
        if (work.second.isNull())
        {
            afterCalculateSurfaceMaster(work.first, nullptr);
            continue;
        }

        BASSERT(!work.second->isEmpty());
        BASSERT(!work.second->isFull());

        t_tilePtr workTile(getTile(work.first));

        m_worker.post(boost::bind(&surface::calculateSurfaceTS, this, work.first, work.second, workTile));
    }
}

void surface::calculateSurfaceTS(const t_tileId id, accessor::t_tilePtr work, t_tilePtr workTile)
{
    if (workTile.isNull())
    {
        workTile = createTile();
    }
    workTile->calculateSurface(work,
                               vector3(id*tile::container::voxelLength)*m_voxelSize,
                               m_voxelSize,
                               true,
                               m_lod);

    if (workTile->getIndices().empty())
    {
        blub::BWARNING("workTile->getIndices().empty() id:" + blub::string::number(id) + " m_lod:" + blub::string::number(m_lod) + " work->getNumVoxelLargerZero():" + blub::string::number(work->getNumVoxelLargerZero()));
        m_master.post(boost::bind(&surface::afterCalculateSurfaceMaster, this, id, nullptr));
        return;
    }

    m_master.post(boost::bind(&surface::afterCalculateSurfaceMaster, this, id, workTile));
}

void surface::afterCalculateSurfaceMaster(const surface::t_tileId &id, surface::t_tilePtr workTile)
{
#ifdef BLUB_LOG_VOXEL_SURFACE
    BOUT("afterCalculateSurfaceMaster id:" + blub::string::number(id));
#endif

    t_tilesMap::const_iterator it(m_tiles.find(id));

    int32 numIndices(0);
    if (!workTile.isNull())
    {
        numIndices = workTile->getIndices().size();
    }

    // no indices
    if (numIndices == 0)
    {
        if (it != m_tiles.cend())
        {
            addToChangeList(id, nullptr);
            m_tiles.erase_return_void(it);
        }
    }
    else
    {
        BASSERT(!workTile.isNull());
        if (it == m_tiles.cend())
        {
            m_tiles.insert(id, workTile);
        }
        addToChangeList(id, workTile);
    }

    --m_numTilesInWork;
    BASSERT(m_numTilesInWork >= 0);
    if (m_numTilesInWork == 0)
    {
        m_voxels.unlockRead();
        t_base::unlockForEditMaster();
    }
}



