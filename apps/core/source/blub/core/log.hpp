#ifndef BLUBLOG_HPP
#define BLUBLOG_HPP

#include "blub/core/globals.hpp"
#include "blub/async/mutex.hpp"
#include "blub/core/string.hpp"

#include <boost/chrono/system_clocks.hpp>
#include <boost/current_function.hpp>
#include <boost/thread/thread.hpp>

#include <fstream>


namespace blub
{

    class log
    {
    public:
        static bool initialise(const string &src);

        static void error(const string& err, const string& file, const uint16& line, const string &function);
        static void out(const string &out, const string &file, const uint16 &line, const string &function);
        static void warning(const string& warn, const string& file, const uint16& line, const string &function);

    private:
        static void writeLine(const string& type, const string &text, const string &file, const uint16 &line, const boost::thread::id& threadId, const string &function);
        static std::ofstream* m_save;
        static async::mutex m_mutex;

        static boost::chrono::system_clock::time_point m_lastOutput;
    };


#define BERROR(x) log::error(x, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#define BWARNING(x) log::warning(x, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#define BOUT(x) log::out(x, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

}

#endif // BLUBLOG_HPP
