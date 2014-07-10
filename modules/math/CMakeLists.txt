
set(sources
log/global.cpp
axisAlignedBox.cpp
colour.cpp
intersection.cpp
math.cpp
plane.cpp
quaternion.cpp
ray.cpp
rectangle.cpp
sphere.cpp
transform.cpp
triangleVector3.cpp
vector2.cpp
vector3.cpp
)

set(headers
log/global.hpp
predecl.hpp
rectangle.hpp
triangleVector3.hpp
colour.hpp
octree/search.hpp
octree/container.hpp
triangle.hpp
vector3.hpp
vector2int.hpp
transform.hpp
axisAlignedBox.hpp
sphere.hpp
ray.hpp
intersection.hpp
vector3int32map.hpp
axisAlignedBoxTemplate.hpp
axisAlignedBoxInt32.hpp
math.hpp
quaternion.hpp
vector2.hpp
plane.hpp
)

set (modules_to_link core log)
add_module(math)
