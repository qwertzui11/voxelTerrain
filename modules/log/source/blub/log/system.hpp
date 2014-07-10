#ifndef BLUB_LOG_SYSTEM_HPP
#define BLUB_LOG_SYSTEM_HPP

#include "blub/core/predecl.hpp"
#include "blub/log/predecl.hpp"



namespace blub
{
namespace log
{
    class system
    {
    public:
        static void addFile(const blub::string &file);
        static void addConsole();

    };
}
}


#endif // BLUB_LOG_SYSTEM_HPP
