
set(sources
log/global.cpp
)

set(headers
predecl.hpp
log/global.hpp
voxel/data.hpp
voxel/edit/axisAlignedBox.hpp
voxel/edit/base.hpp
voxel/edit/box.hpp
voxel/edit/mesh.hpp
voxel/edit/noise.hpp
voxel/edit/sphere.hpp
voxel/terrain/base.hpp
voxel/terrain/accessor.hpp
voxel/terrain/surface.hpp
voxel/terrain/renderer.hpp
voxel/simple/base.hpp
voxel/simple/container/base.hpp
voxel/simple/container/inMemory.hpp
voxel/simple/container/utils/tile.hpp
voxel/simple/accessor.hpp
voxel/simple/surface.hpp
voxel/simple/renderer.hpp
voxel/tile/internal/transvoxelTables.hpp
voxel/tile/accessor.hpp
voxel/tile/base.hpp
voxel/tile/surface.hpp
voxel/tile/container.hpp
voxel/tile/renderer.hpp
)

add_module(procedural)
