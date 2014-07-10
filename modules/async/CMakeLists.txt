message("doing async")

set(sources
log/global.cpp
dispatcher.cpp
deadlineTimer.cpp
seperate.cpp
updater.cpp
PARENT_SCOPE
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
PARENT_SCOPE
)
