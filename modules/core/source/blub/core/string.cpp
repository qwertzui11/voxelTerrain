#include "blub/core/string.hpp"

#include "blub/core/byteArray.hpp"

#include "blub/math/quaternion.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/transform.hpp"
#include "blub/math/triangle.hpp"
#include "blub/math/triangleVector3.hpp"
#include "blub/math/vector2.hpp"
#include "blub/math/vector3int32.hpp"

#include <ostream>


/*#ifndef NO_OGRE3D
#include <OGRE/OgreString.h>
#endif*/


using namespace blub;


string::string(const byteArray &array)
    : t_base(array.data(), array.size())
{

}

string string::number(const axisAlignedBox& aab)
{
    return number(aab.getMinimum()) + "; " + number(aab.getMaximum());
}

string string::number(const vector3int32& vec)
{
    return "(" + number(vec.x) + "," + number(vec.y) + "," + number(vec.z) + ")";
}

string string::number(const vector3& vec)
{
    return number(vec.x) + "," + number(vec.y) + "," + number(vec.z);
}

string string::number(const vector2& vec)
{
    return number(vec.x) + "," + number(vec.y);
}

string string::number(const quaternion &quat)
{
    return "(" + number(quat.x) + ", " + number(quat.y) + ", " + number(quat.z) + ", " + number(quat.w) + ")";
}

string string::number(const transform &trans)
{
    return "{" + number(trans.position) + "," + number(trans.rotation) + "," + number(trans.scale) + "}";
}



std::ostream &operator <<(std::ostream &ostr, const string &toCast)
{
    return ostr << static_cast<std::string>(toCast);
}
