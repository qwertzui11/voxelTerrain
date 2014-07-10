#include "compression.hpp"

#include "blub/core/byteArray.hpp"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>


using namespace blub;


byteArray compression::compressBz2(const byteArray &work)
{
    std::stringstream dataContainerCompressed;
    {
        boost::iostreams::filtering_ostream filterOut;
        filterOut.push(boost::iostreams::bzip2_compressor());
        filterOut.push(dataContainerCompressed);

        filterOut.write(work.data(), work.size());
    }

    const byteArray result(dataContainerCompressed.str().data(), dataContainerCompressed.str().size());
    return result;
}

byteArray compression::decompressBz2(const byteArray &work)
{
    boost::iostreams::stream<boost::iostreams::array_source> src(work.data(), work.size());
    boost::iostreams::filtering_istream filterIn;
    filterIn.push(boost::iostreams::bzip2_decompressor());
    filterIn.push(src);

    std::stringstream resultBuffer;
    boost::iostreams::copy(filterIn, resultBuffer);

    const byteArray result(resultBuffer.str().data(), resultBuffer.str().size());
    return result;
}
