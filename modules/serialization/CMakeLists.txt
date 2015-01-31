
set(sources
log/global.cpp
format/binary/portable_binary_oarchive.cpp
format/binary/portable_binary_iarchive.cpp
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
format/binary/portable_binary_archive.hpp
format/binary/output.hpp
format/binary/input.hpp
format/binary/portable_binary_oarchive.hpp
format/binary/portable_binary_iarchive.hpp
format/xml/output.hpp
format/xml/input.hpp
)

set (modules_to_link log)
add_module(serialization)
