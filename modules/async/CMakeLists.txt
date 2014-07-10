
set(sources
log/global.cpp
dispatcher.cpp
deadlineTimer.cpp
seperate.cpp
updater.cpp
)

set(headers
log/global.hpp
deadlineTimer.hpp
dispatcher.hpp
mutex.hpp
mutexLocker.hpp
mutexReadWrite.hpp
predecl.hpp
seperate.hpp
updater.hpp
)

set (modules_to_link core log)
add_module(async)
