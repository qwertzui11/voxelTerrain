#include "rectangle.hpp"

using namespace blub;

rectangle::rectangle()
{
}

rectangle::rectangle(const vector2 &topLeft_, const vector2 &rightBottom_)
    : topLeft(topLeft_)
    , rightBottom(rightBottom_)
{
}

void rectangle::merge(const rectangle &other)
{
    topLeft.x   = other.topLeft.x   > topLeft.x   ? other.topLeft.x   : topLeft.x;
    topLeft.y    = other.topLeft.y    > topLeft.y    ? other.topLeft.y    : topLeft.y;
    rightBottom.x  = other.rightBottom.x  < rightBottom.x  ? other.rightBottom.x  : rightBottom.x;
    rightBottom.y = other.rightBottom.y < rightBottom.y ? other.rightBottom.y : rightBottom.y;
}

void rectangle::merge(const vector2 &other)
{
    topLeft.x   = other.x   > topLeft.x   ? other.x   : topLeft.x;
    topLeft.y    = other.y    > topLeft.y    ? other.y    : topLeft.y;
    rightBottom.x  = other.x  < rightBottom.x  ? other.x  : rightBottom.x;
    rightBottom.y = other.y < rightBottom.y ? other.y : rightBottom.y;
}
