
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
  voxel/container/multipleTiles/network_sync_voxel_container_multipleTiles_receiver.cpp
  voxel/container/multipleTiles/network_sync_voxel_container_multipleTiles_sender.cpp
  )
  set(headers ${headers}
  voxel/accessor/multipleTiles/receiver.hpp
  voxel/accessor/multipleTiles/base.hpp
  voxel/accessor/multipleTiles/sender.hpp
  voxel/accessor/terrain/receiver.hpp
  voxel/accessor/terrain/sender.hpp
  voxel/container/multipleTiles/receiver.hpp
  voxel/container/multipleTiles/network_sync_voxel_container_multipleTiles_receiver.hpp
  voxel/container/multipleTiles/base.hpp
  voxel/container/multipleTiles/sender.hpp
  voxel/container/multipleTiles/network_sync_voxel_container_multipleTiles_sender.hpp
  )
endif (BLUB_BUILD_PROCEDURAL)

add_module(sync)

