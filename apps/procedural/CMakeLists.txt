message("doing procedural")

set(sources
voxel/edit/axisAlignedBox.cpp
voxel/edit/base.cpp
voxel/edit/sphere.cpp
voxel/terrain/accessor.cpp
voxel/terrain/surface.cpp
voxel/terrain/renderer.cpp
voxel/simple/container/base.cpp
voxel/simple/container/inMemory.cpp
voxel/simple/accessor.cpp
voxel/simple/surface.cpp
voxel/simple/renderer.cpp
voxel/tile/accessor.cpp
voxel/tile/surface.cpp
voxel/tile/container.cpp
voxel/tile/renderer.cpp
PARENT_SCOPE
)

set(headers
predecl.hpp
voxel/data.hpp
voxel/edit/base.hpp
voxel/edit/axisAlignedBox.hpp
voxel/edit/sphere.hpp
voxel/terrain/base.hpp
voxel/terrain/accessor.hpp
voxel/terrain/surface.hpp
voxel/terrain/renderer.hpp
voxel/simple/base.hpp
voxel/simple/container/utils/tile.hpp
voxel/simple/container/base.hpp
voxel/simple/container/inMemory.hpp
voxel/simple/accessor.hpp
voxel/simple/surface.hpp
voxel/simple/renderer.hpp
voxel/tile/internal/transvoxelTables.hpp
voxel/tile/accessor.hpp
voxel/tile/base.hpp
voxel/tile/surface.hpp
voxel/tile/container.hpp
voxel/tile/renderer.hpp
PARENT_SCOPE
)


