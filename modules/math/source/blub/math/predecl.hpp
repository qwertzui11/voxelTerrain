#ifndef BLUB_MATH_PREDECL_HPP
#define BLUB_MATH_PREDECL_HPP

#include "blub/core/globals.hpp"


namespace blub
{


class axisAlignedBox;
class axisAlignedBoxInt32;
template <class vector3Type>
class axisAlignedBoxTemplate;
class colour;
class plane;
class quaternion;
class rectangle;
class sphere;
class transform;
class triangle;
class triangleVector3;
class vector2;
template <typename valueType, valueType valueDefault = 0>
class vector2Template;
typedef vector2Template<int32, 0> vector2int32;
class vector3;
template <typename valueType, valueType valueDefault = 0>
class vector3Template;
typedef vector3Template<int32, 0> vector3int32;
typedef vector3Template<uint8, 0> vector3uint8;



}


#endif // BLUB_MATH_PREDECL_HPP
