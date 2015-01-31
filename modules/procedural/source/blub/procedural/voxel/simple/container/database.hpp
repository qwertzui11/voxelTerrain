#ifndef BLUB_PROCEDURAL_VOXEL_SIMPLE_CONTAINER_DATABASE_HPP
#define BLUB_PROCEDURAL_VOXEL_SIMPLE_CONTAINER_DATABASE_HPP

//#ifdef BLUB_BUILD_DATABASE

// #include "blub/log/global.hpp"
#include "blub/core/base64.hpp"
#include "blub/core/byteArray.hpp"
#include "blub/core/vector.hpp"
#include "blub/database/connection.hpp"
#include "blub/database/functions.hpp"
#include "blub/procedural/log/global.hpp"
#include "blub/procedural/voxel/simple/container/inMemory.hpp"
#include "blub/procedural/voxel/simple/container/utils/tile.hpp"
#include "blub/serialization/format/binary/input.hpp"
#include "blub/serialization/format/binary/output.hpp"

#include <functional>
#include <boost/interprocess/streams/vectorstream.hpp>


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


template <class voxelType>
class database : public inMemory<voxelType>
{
public:
    typedef inMemory<voxelType> t_base;
    typedef std::function<byteArray (const byteArray&)> t_funcCompress;

    database(blub::async::dispatcher &worker, blub::database::connection& dbConn)
        : t_base(worker)
        , m_databaseConnection(dbConn)
        , m_loading(false)
        , m_writeToDatabase(true)
    {
#ifdef BLUB_LOG_VOXEL
        blub::BOUT("database::database()");
#endif

    }
    ~database()
    {
    #ifdef BLUB_LOG_VOXEL
        blub::BOUT("database::~database()");
    #endif
    }

    void loadTS()
    {
        t_base::m_master.dispatch(boost::bind(&database::loadMaster, this));
    }

    void loadMaster()
    {
        m_loading = true;
        uint32 numData;
        m_databaseConnection << "select count(*) from voxel_tiles"
                                , blub::database::into(numData);
        if (numData == 0)
        {
			BLUB_PROCEDURAL_LOG_WARNING() << "numData == 0";
            return;
        }
        std::vector<int32> xList(numData);
        std::vector<int32> yList(numData);
        std::vector<int32> zList(numData);
        m_databaseConnection << "select x, y, z from voxel_tiles"
                                , soci::into(xList), soci::into(yList), soci::into(zList);
        BASSERT(xList.size() == yList.size());
        BASSERT(xList.size() == zList.size());
        t_base::lockForEditMaster();
        for (uint32 ind = 0; ind < xList.size(); ++ind)
        {
            const vector3int32 id(xList[ind], yList[ind], zList[ind]);
            typename t_base::t_utilsTile tile(getTileHolderFromDatabaseMaster(id));
            t_base::setTileMaster(id, tile);
        }
        m_loading = false;
        t_base::unlockForEditMaster();
    }

    byteArray getTileHolderDatabaseMaster(const blub::vector3int32& id) const
    {
        soci::indicator indicator = soci::i_null;
        std::string selectData;
        m_databaseConnection << "select data from voxel_tiles where x = :x and y = :y and z = :z"
                                , soci::into(selectData, indicator), blub::database::use(id.x), blub::database::use(id.y), blub::database::use(id.z);
        if (indicator == soci::i_ok)
        {
            byteArray decompressed(base64::decode(selectData));
            if (m_funcDecompress)
            {
                decompressed = m_funcDecompress(decompressed);
            }

            return decompressed;
        }
        return byteArray();
    }

    typename t_base::t_utilsTile getTileHolderFromDatabaseMaster(const blub::vector3int32& id) const
    {
        typename t_base::t_utilsTile result;

        const byteArray &strToCast(getTileHolderDatabaseMaster(id));

        if (!strToCast.empty())
        {
            boost::interprocess::basic_ivectorstream<byteArray> dataContainer(strToCast);
//            std::istringstream dataContainer(std::string(strToCast.data(), strToCast.size()));
            {
                blub::serialization::format::binary::input format(dataContainer);
                format >> result.state;
                if (result.state == utils::tileState::partitial)
                {
                    result.data = t_base::createTileFull(false);

                    format >> (*result.data.data());
                }
            }
        }

        return result;
    }


    void setWriteToDatabase(const bool& en)
    {
        m_writeToDatabase = en;
    }
    void setCompressionCallback(const t_funcCompress& compress, const t_funcCompress& decompress)
    {
        m_funcCompress = compress;
        m_funcDecompress = decompress;
    }


protected:
    virtual void setTileDatabaseMaster(const vector3int32 &id, const blub::byteArray &toSet, const bool &insert, const bool &update, const bool &remove)
    {
#ifdef BLUB_DEBUG
        if (remove)
        {
            BASSERT(!update);
            BASSERT(!insert);
        }
        if (insert)
        {
            BASSERT(!update);
            BASSERT(!remove);
        }
        if (update)
        {
            BASSERT(!insert);
            BASSERT(!remove);
        }
#endif

        const byteArray *work(&toSet);
        byteArray toSetCompressed;
        if (m_funcCompress)
        {
            toSetCompressed = m_funcCompress(toSet);
            work = &toSetCompressed;
        }
        const blub::string toSetBase64(base64::encode(*work));


        if (insert)
        {
            m_databaseConnection << "insert into voxel_tiles(x, y, z, data) values(:x, :y, :z, :data)"
                                    , blub::database::use(id.x), blub::database::use(id.y), blub::database::use(id.z)
                                    , blub::database::use(toSetBase64);
            return;
        }
        if (remove)
        {
            m_databaseConnection << "delete data from voxel_tiles where x = :x and y = :y and z = :z"
                                    , blub::database::use(id.x), blub::database::use(id.y), blub::database::use(id.z);
            return;
        }
        if (update)
        {
            m_databaseConnection << "update voxel_tiles set data = :data where x = :x and y = :y and z = :z"
                                    , blub::database::use(toSetBase64), blub::database::use(id.x), blub::database::use(id.y), blub::database::use(id.z);
            return;
        }
    }

    void setTileToContainerMaster(const typename t_base::t_tileId id,
                                  const typename t_base::t_utilsTile &oldOne,
                                  const typename t_base::t_utilsTile &toSet) override
    {
        t_base::setTileToContainerMaster(id, oldOne, toSet);

        if (m_loading || !m_writeToDatabase)
        {
            return;
        }
        std::stringstream dataContainer;
        {
            blub::serialization::format::binary::output format(dataContainer);
            format << toSet.state;
            if (toSet.state == utils::tileState::partitial)
            {
                BASSERT(!toSet.data.isNull());
                format << *toSet.data.data();
            }
        }
        const blub::byteArray strCasted(dataContainer.str());

        setTileDatabaseMaster(id, strCasted,
                              oldOne.state == utils::tileState::empty,
                              (toSet.state != utils::tileState::empty) && (oldOne.state != utils::tileState::empty),
                              toSet.state == utils::tileState::empty);
    }


protected:
    blub::database::connection &m_databaseConnection;
    bool m_loading;
    bool m_writeToDatabase;
    t_funcCompress m_funcCompress;
    t_funcCompress m_funcDecompress;

private:

};


}
}
}
}
}

//#endif // BLUB_BUILD_DATABASE

#endif // BLUB_PROCEDURAL_VOXEL_SIMPLE_CONTAINER_DATABASE_HPP
