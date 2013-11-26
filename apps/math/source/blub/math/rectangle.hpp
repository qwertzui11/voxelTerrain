#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "vector2.hpp"

namespace blub
{

class rectangle
{
public:
    vector2 topLeft, rightBottom;

    rectangle(void);
    rectangle(const vector2 &topLeft_, const vector2 &rightBottom_);

    void merge(const rectangle &other);
    void merge(const vector2 &other);
};

}

#endif // RECTANGLE_HPP
