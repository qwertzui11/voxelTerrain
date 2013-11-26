#include "log.hpp"

#include "blub/core/dateTime.hpp"
#include "blub/core/string.hpp"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>


using namespace blub;


std::ofstream *log::m_save;
async::mutex log::m_mutex;
boost::chrono::system_clock::time_point log::m_lastOutput;

void log::error(const string& err, const string& file, const uint16& line, const string& function)
{
    boost::thread::id threadId(boost::this_thread::get_id());
    writeLine("ERR", err, file, line, threadId, function);
}

void log::warning(const string& warn, const string& file, const uint16& line, const string& function)
{
    boost::thread::id threadId(boost::this_thread::get_id());
    writeLine("WAR", warn, file, line, threadId, function);
}

void log::out(const string& out, const string& file, const uint16& line, const string& function)
{
    boost::thread::id threadId(boost::this_thread::get_id());
    writeLine("OUT", out, file, line, threadId, function);
}

bool log::initialise(const string& src)
{
    m_lastOutput = boost::chrono::system_clock::now();
    m_save = new std::ofstream(src.data());
    BASSERT(m_save->is_open());

    return true;
}

void log::writeLine(const string& type, const string &text, const string &file, const uint16 &line, const boost::thread::id &threadId, const string& function)
{
    std::stringstream ios;
    void* numericId;
    ios << threadId;
    ios >> numericId;

    boost::chrono::duration<double> duration = boost::chrono::system_clock::now() - m_lastOutput;
    m_lastOutput = boost::chrono::system_clock::now();

    BASSERT(m_save->is_open());
    (*m_save) <<
                            type << " " <<
                            dateTime::getUniversalTimeString() << " " << std::fixed <<  std::setprecision(8) << duration.count() << " " <<
                            numericId << "\t" <<
                            text << "\t" <<
                            file << ":" << line << "\t" <<
                            function
#ifdef BLUB_DEBUG
                            << std::endl;
#else
                            << '\n';
#endif
}
