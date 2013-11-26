#include "blub/math/sphere.hpp"

#include "axisAlignedBox.hpp"


using namespace blub;


bool sphere::intersects(const sphere &s) const
{
    return (s.mCenter - mCenter).squaredLength() <= (s.mRadius + mRadius)*(s.mRadius + mRadius);
}

void sphere::merge(const sphere& oth)
{
    vector3 diff = oth.getCenter() - mCenter;
    real lengthSq = diff.squaredLength();
    real radiusDiff = oth.getRadius() - mRadius;

    // Early-out
    if (radiusDiff*radiusDiff >= lengthSq)
    {
        // One fully contains the other
        if (radiusDiff <= 0.0f)
            return; // no change
        else
        {
            mCenter = oth.getCenter();
            mRadius = oth.getRadius();
            return;
        }
    }

    real length = math::sqrt(lengthSq);
    real t = (length + radiusDiff) / (2.0f * length);
    mCenter = mCenter + diff * t;
    mRadius = 0.5f * (length + mRadius + oth.getRadius());
}

bool sphere::inherts(axisAlignedBox aabb) const
{
   return aabb.getMinimum().squaredDistance(getCenter()) < getRadius()*getRadius() &&
          aabb.getMaximum().squaredDistance(getCenter()) < getRadius()*getRadius();
}

bool sphere::intersects(const axisAlignedBox &box) const
{
    if (box.isNull()) return false;
    if (box.isInfinite()) return true;

    // Use splitting planes
    const vector3& center = getCenter();
    real radius = getRadius();
    const vector3& min = box.getMinimum();
    const vector3& max = box.getMaximum();

    // Arvo's algorithm
    real s, d = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (center.ptr()[i] < min.ptr()[i])
        {
            s = center.ptr()[i] - min.ptr()[i];
            d += s * s;
        }
        else if(center.ptr()[i] > max.ptr()[i])
        {
            s = center.ptr()[i] - max.ptr()[i];
            d += s * s;
        }
    }
    return d <= radius * radius;
}
