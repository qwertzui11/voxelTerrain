


set(sources
noise.cpp
primitives.cpp
)

if (BLUB_USE_ASSIMP)
  set(sources ${sources} mesh.cpp)
endif(BLUB_USE_ASSIMP)

set(headers
Handler.hpp
OgreTile.hpp
)

add_example(voxelterrain)

