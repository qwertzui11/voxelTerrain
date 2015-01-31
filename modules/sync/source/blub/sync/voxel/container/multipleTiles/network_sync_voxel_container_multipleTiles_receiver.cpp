#include "network_sync_voxel_container_multipleTiles_receiver.hpp"

#include "blub/core/byteArray.hpp"
#include "blub/log/global.hpp"
#include "blub/async/dispatcher.hpp"
#include "blub/sync/voxel/container/multipleTiles/base.hpp"
#include "blub/procedural/voxel/simple/container/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/serialization/format/binary/input.hpp"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>


using namespace blub::sync::voxel::container::simple;
using namespace blub;


//receiver::receiver(async::dispatcher *todoListenerMaster, procedural::voxel::simple::container::base *tiles)
//    : m_tiles(tiles)
//    , m_todoListenerMaster(todoListenerMaster)
//{
//    if (m_tiles)
//    {
//        signalStartEditing()->connect(boost::bind(&procedural::voxel::simple::container::base::lockForEdit, m_tiles));
//        signalEndEditing()->connect(boost::bind(&procedural::voxel::simple::container::base::unlockForEdit, m_tiles));
//        signalSetTile()->connect(boost::bind(&procedural::voxel::simple::container::base::setTile, m_tiles, _1, _2));
//    }
//}

//void receiver::receivedTileData(const byteArray &data)
//{
//    m_todoListenerMaster->post(boost::bind(&receiver::receivedTileDataMaster, this, data));
//}

//void receiver::receivedTilePtrData(receiver::t_tileDataPtr data)
//{
//    BASSERT(!data.isNull());

//    m_todoListenerMaster->post(boost::bind(&receiver::receivedTileDataPtrMaster, this, data));
//}

//receiver::t_sigStartEditing *receiver::signalStartEditing()
//{
//    return &m_sigStartEditing;
//}

//receiver::t_sigEndEditing *receiver::signalEndEditing()
//{
//    return &m_sigEndEditing;
//}

//receiver::t_sigSetTile *receiver::signalSetTile()
//{
//    return &m_sigSetTile;
//}

//void receiver::receivedTileDataMaster(const byteArray &data)
//{
//    sendType type;
//    t_tileId id;

//    BASSERT((uint32)data.size() >= sizeof(sendType));
//    memcpy(&type, data.data(), sizeof(sendType));

//    if (type == sendType::startEditing)
//    {
//        // blub::BOUT("type == sendType::startEditing");
//        m_sigStartEditing();
//        return;
//    }
//    if (type == sendType::endEditing)
//    {
//        // blub::BOUT("type == sendType::endEditing");
//        m_sigEndEditing();
//        return;
//    }

//    BASSERT(type == sendType::setTile);
//    typedef procedural::voxel::simple::container::utils::tileState t_tileState;
//    typedef procedural::voxel::simple::container::utils::tile t_tileHolder;
//    BASSERT((uint32)data.size() >= sizeof(sendType) + sizeof(t_tileId) + sizeof(t_tileState));
//    memcpy(&id, data.data()+sizeof(sendType), sizeof(t_tileId));
//    t_tileState tileState_;
//    memcpy(&tileState_, data.data()+sizeof(sendType) + sizeof(t_tileId), sizeof(t_tileState));

//    if (tileState_ == t_tileState::full)
//    {
//        m_sigSetTile(id, t_tileHolder(t_tileState::full));
//        return;
//    }
//    if (tileState_ == t_tileState::empty)
//    {
//        m_sigSetTile(id, t_tileHolder(t_tileState::empty));
//        return;
//    }
//    t_tilePtr workTile(procedural::voxel::tile::container::create());

//    BASSERT(data.size()-(sizeof(sendType)+sizeof(t_tileId)) >= sizeof(int32));
//    {
//        boost::iostreams::stream<boost::iostreams::array_source> src(   data.data() + sizeof(sendType)+sizeof(t_tileId)+sizeof(t_tileState),
//                                                                        data.size() - (sizeof(sendType)+sizeof(t_tileId)+sizeof(t_tileState)));
//        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//        in.push(boost::iostreams::bzip2_decompressor());
//        in.push(src);
//        serialization::format::binary::input format_(src);
//        {
//            format_ >> (*workTile.data());
//        }
//    }
//    t_tileHolder holder(t_tileState::partitial);
//    holder.data = workTile;
//    m_sigSetTile(id, holder);
//}

//void receiver::receivedTileDataPtrMaster(receiver::t_tileDataPtr data)
//{
//    receivedTileDataMaster(*data.data());
//}
