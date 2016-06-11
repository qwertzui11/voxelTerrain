
set(sources
log/global.cpp
receiver.cpp
)

set(headers
predecl.hpp
log/global.hpp
receiver.hpp
sender.hpp
identifier.hpp
)

if (BLUB_BUILD_PROCEDURAL)
  set(sources ${sources}
  )
  set(headers ${headers}
  voxel/accessor/multipleTiles/receiver.hpp
  voxel/accessor/multipleTiles/base.hpp
  voxel/accessor/multipleTiles/sender.hpp
  voxel/accessor/terrain/receiver.hpp
  voxel/accessor/terrain/sender.hpp
  voxel/container/multipleTiles/receiver.hpp
  voxel/container/multipleTiles/base.hpp
  voxel/container/multipleTiles/sender.hpp
  )
endif (BLUB_BUILD_PROCEDURAL)

add_module(sync)

