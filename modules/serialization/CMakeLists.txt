
set(sources
log/global.cpp
)

set(headers
log/global.hpp
predecl.hpp
saveLoad.hpp
access.hpp
callBaseObject.hpp
nameValuePair.hpp
format/text/output.hpp
format/text/input.hpp
format/xml/output.hpp
format/xml/input.hpp
)

set (modules_to_link log)
add_module(serialization)
