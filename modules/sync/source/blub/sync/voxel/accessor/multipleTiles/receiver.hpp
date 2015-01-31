#ifndef NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_RECEIVER_HPP
#define NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_RECEIVER_HPP

#include "blub/core/byteArray.hpp"
#include "blub/log/global.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/sync/voxel/accessor/multipleTiles/base.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"
#include "blub/procedural/voxel/simple/base.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>



namespace blub
{
namespace sync
{
namespace voxel
{
namespace accessor
{
namespace multipleTiles
{


template <class voxelType>
class receiver : public procedural::voxel::simple::base<sharedPointer<procedural::voxel::tile::accessor<voxelType> > >
{
public:
    typedef sharedPointer<byteArray> t_tileDataPtr;
    typedef procedural::voxel::tile::accessor<voxelType> t_tileAccessor;
    typedef sharedPointer<t_tileAccessor> t_tilePtr;
    typedef procedural::voxel::simple::base<t_tilePtr> t_base;
    typedef typename t_base::t_tileId t_tileId;


    receiver(blub::async::dispatcher * todoListenerMaster, const int32& lod = 0)
        : t_base(*todoListenerMaster)
        , m_lod(lod)
    {
        t_base::setCreateTileCallback(boost::bind(&t_tileAccessor::create));
    }

    // "receive sync"
    void receivedTileData(const byteArray& data)
    {
        t_base::m_master.post(boost::bind(&receiver::receivedTileDataMaster, this, data));
    }
    void receivedTilePtrData(t_tileDataPtr data)
    {
        BASSERT(!data.isNull());

        t_base::m_master.post(boost::bind(&receiver::receivedTileDataPtrMaster, this, data));
    }

protected:
    void receivedTileDataMaster(const byteArray& data)
    {
        sendType type;

        std::istringstream input(std::string(data.data(), data.size()));
        blub::serialization::format::binary::input format(input);
        format >> type;

        if (type == sendType::lockForEdit)
        {
#ifdef BLUB_LOG_VOXEL_ACCESSOR_SYNC
            blub::BOUT("type == sendType::lockForEdit id:" + blub::string::number(id));
#endif
            t_base::lockForEditMaster();
            return;
        }
        if (type == sendType::unlockForEdit)
        {
#ifdef BLUB_LOG_VOXEL_ACCESSOR_SYNC
            blub::BOUT("type == sendType::unlockForEdit id:" + blub::string::number(id));
#endif
            t_base::unlockForEditMaster();
            return;
        }

        t_tileId id;
        format >> id;

        if (type == sendType::removeTile)
        {
#ifdef BLUB_LOG_VOXEL_ACCESSOR_SYNC
            blub::BOUT("type == sendType::removeTile id:" + blub::string::number(id));
#endif
            t_base::addToChangeList(id, nullptr);
            return;
        }

        BASSERT((uint32)data.size() > sizeof(sendType) + sizeof(t_tileId));
        BASSERT(type == sendType::setTile);
#ifdef BLUB_LOG_VOXEL_ACCESSOR_SYNC
        blub::BOUT("type == sendType::setTile id:" + blub::string::number(id));
#endif

        byteArray toDecompress;
        format >> toDecompress;

        t_tilePtr workTile(t_base::createTile());
        {
            std::stringstream toReadFromBuffer;
            // decompress
            {
                boost::iostreams::stream<boost::iostreams::array_source> src(toDecompress.data(), toDecompress.size());
                boost::iostreams::filtering_istream filterIn;
                filterIn.push(boost::iostreams::bzip2_decompressor());
                filterIn.push(src);
                boost::iostreams::copy(filterIn, toReadFromBuffer);
            }

            blub::serialization::format::binary::input toReadFrom(toReadFromBuffer);

            toReadFrom >> *workTile.get();
        }
        t_base::addToChangeList(id, workTile);
    }
    void receivedTileDataPtrMaster(t_tileDataPtr data)
    {
        receivedTileDataMaster(*data.data());
    }

    const int32 m_lod;

};




}
}
}
}
}





#endif // NETWORK_SYNC_VOXEL_ACCESSOR_MULTIPLETILES_RECEIVER_HPP
