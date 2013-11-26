message("doing async")

set(sources
dispatcher.cpp
deadlineTimer.cpp
seperate.cpp
updater.cpp
PARENT_SCOPE
)

set(headers
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
