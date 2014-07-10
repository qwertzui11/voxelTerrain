#ifndef AXISALIGNEDBOX_HPP
#define AXISALIGNEDBOX_HPP

#include "blub/core/classVersion.hpp"
#include "blub/math/vector3.hpp"
#include "blub/serialization/nameValuePair.hpp"
#include "blub/serialization/saveLoad.hpp"


namespace Ogre
{
    class AxisAlignedBox;
}


namespace blub
{


class axisAlignedBox
{
public:
    enum Extent
    {
        EXTENT_NULL,
        EXTENT_FINITE,
        EXTENT_INFINITE
    };
protected:

    vector3 mMinimum;
    vector3 mMaximum;
    Extent mExtent;

public:
    /*
       1-----2
      /|    /|
     / |   / |
    5-----4  |
    |  0--|--3
    | /   | /
    |/    |/
    6-----7
    */
    typedef enum {
        FAR_LEFT_BOTTOM = 0,
        FAR_LEFT_TOP = 1,
        FAR_RIGHT_TOP = 2,
        FAR_RIGHT_BOTTOM = 3,
        NEAR_RIGHT_BOTTOM = 7,
        NEAR_LEFT_BOTTOM = 6,
        NEAR_LEFT_TOP = 5,
        NEAR_RIGHT_TOP = 4
    } CornerEnum;

    axisAlignedBox(const axisAlignedBoxInt32 &toCast);
#ifndef BLUB_NO_OGRE3D
    axisAlignedBox(const Ogre::AxisAlignedBox &vec);
    operator Ogre::AxisAlignedBox() const;
#endif

    inline axisAlignedBox() : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
    {
        // Default to a null box
        setMinimum( -0.5, -0.5, -0.5 );
        setMaximum( 0.5, 0.5, 0.5 );
        mExtent = EXTENT_NULL;
    }
    inline axisAlignedBox(Extent e) : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
    {
        setMinimum( -0.5, -0.5, -0.5 );
        setMaximum( 0.5, 0.5, 0.5 );
        mExtent = e;
    }


    inline axisAlignedBox(const axisAlignedBox & rkBox) : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)

    {
        if (rkBox.isNull())
            setNull();
        else if (rkBox.isInfinite())
            setInfinite();
        else
            setExtents( rkBox.mMinimum, rkBox.mMaximum );
    }

    inline axisAlignedBox( const vector3& min, const vector3& max )
        : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
    {
        setExtents( min, max );
    }

    inline axisAlignedBox(
                    real mx, real my, real mz,
                    real Mx, real My, real Mz)
        : mMinimum(vector3::ZERO), mMaximum(vector3::UNIT_SCALE)
    {
        setExtents( mx, my, mz, Mx, My, Mz );
    }


    axisAlignedBox& operator=(const axisAlignedBox& rhs)
    {
        // Specifically override to avoid copying mCorners
        if (rhs.isNull())
            setNull();
        else if (rhs.isInfinite())
            setInfinite();
        else
            setExtents(rhs.mMinimum, rhs.mMaximum);

        return *this;
    }

    ~axisAlignedBox()
    {
        //if (mCorners)
        //    OGRE_FREE(mCorners, MEMCATEGORY_SCENE_CONTROL);
    }


    /** Gets the minimum corner of the box.
     */
    inline const vector3& getMinimum(void) const
    {
        return mMinimum;
    }

    /** Gets a modifiable version of the minimum
     * corner of the box.
     */
    inline vector3& getMinimum(void)
    {
        return mMinimum;
    }

    /** Gets the maximum corner of the box.
     */
    inline const vector3& getMaximum(void) const
    {
        return mMaximum;
    }

    /** Gets a modifiable version of the maximum
     * corner of the box.
     */
    inline vector3& getMaximum(void)
    {
        return mMaximum;
    }


    /** Sets the minimum corner of the box.
    */
    inline void setMinimum( const vector3& vec )
    {
        mExtent = EXTENT_FINITE;
        mMinimum = vec;
    }

    inline void setMinimum( real x, real y, real z )
    {
        mExtent = EXTENT_FINITE;
        mMinimum.x = x;
        mMinimum.y = y;
        mMinimum.z = z;
    }

    /** Changes one of the components of the minimum corner of the box
    * used to resize only one dimension of the box
    */
    inline void setMinimumX(real x)
    {
        mMinimum.x = x;
    }

    inline void setMinimumY(real y)
    {
        mMinimum.y = y;
    }

    inline void setMinimumZ(real z)
    {
        mMinimum.z = z;
    }

    /** Sets the maximum corner of the box.
    */
    inline void setMaximum( const vector3& vec )
    {
        mExtent = EXTENT_FINITE;
        mMaximum = vec;
    }

    inline void setMaximum( real x, real y, real z )
    {
        mExtent = EXTENT_FINITE;
        mMaximum.x = x;
        mMaximum.y = y;
        mMaximum.z = z;
    }

    /** Changes one of the components of the maximum corner of the box
     * used to resize only one dimension of the box
    */
    inline void setMaximumX( real x )
    {
        mMaximum.x = x;
    }

    inline void setMaximumY( real y )
    {
        mMaximum.y = y;
    }

    inline void setMaximumZ( real z )
    {
        mMaximum.z = z;
    }

    /** Sets both minimum and maximum extents at once.
    */
    inline void setExtents( const vector3& min, const vector3& max )
    {
        BASSERT( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
            "The minimum corner of the box must be less than or equal to maximum corner" );

        mExtent = EXTENT_FINITE;
        mMinimum = min;
        mMaximum = max;
    }

    inline void setExtents(
        real mx, real my, real mz,
        real Mx, real My, real Mz )
    {
        BASSERT( (mx <= Mx && my <= My && mz <= Mz) &&
            "The minimum corner of the box must be less than or equal to maximum corner" );

        mExtent = EXTENT_FINITE;

        mMinimum.x = mx;
        mMinimum.y = my;
        mMinimum.z = mz;

        mMaximum.x = Mx;
        mMaximum.y = My;
        mMaximum.z = Mz;

    }

    /** gets the position of one of the corners
    */
    vector3 getCorner(CornerEnum cornerToGet) const
    {
        switch(cornerToGet)
        {
        case FAR_LEFT_BOTTOM:
            return mMinimum;
        case FAR_LEFT_TOP:
            return vector3(mMinimum.x, mMaximum.y, mMinimum.z);
        case FAR_RIGHT_TOP:
            return vector3(mMaximum.x, mMaximum.y, mMinimum.z);
        case FAR_RIGHT_BOTTOM:
            return vector3(mMaximum.x, mMinimum.y, mMinimum.z);
        case NEAR_RIGHT_BOTTOM:
            return vector3(mMaximum.x, mMinimum.y, mMaximum.z);
        case NEAR_LEFT_BOTTOM:
            return vector3(mMinimum.x, mMinimum.y, mMaximum.z);
        case NEAR_LEFT_TOP:
            return vector3(mMinimum.x, mMaximum.y, mMaximum.z);
        case NEAR_RIGHT_TOP:
            return mMaximum;
        default:
            return vector3();
        }
    }

    /** Merges the passed in box into the current box. The result is the
     * box which encompasses both.
    */
    void merge( const axisAlignedBox& rhs )
    {
        // Do nothing if rhs null, or this is infinite
        if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
        {
            return;
        }
        // Otherwise if rhs is infinite, make this infinite, too
        else if (rhs.mExtent == EXTENT_INFINITE)
        {
            mExtent = EXTENT_INFINITE;
        }
        // Otherwise if current null, just take rhs
        else if (mExtent == EXTENT_NULL)
        {
            setExtents(rhs.mMinimum, rhs.mMaximum);
        }
        // Otherwise merge
        else
        {
            vector3 min = mMinimum;
            vector3 max = mMaximum;
            max.makeCeil(rhs.mMaximum);
            min.makeFloor(rhs.mMinimum);

            setExtents(min, max);
        }

    }

    /** Extends the box to encompass the specified point (if needed).
    */
    inline void merge( const vector3& point )
    {
        switch (mExtent)
        {
        case EXTENT_NULL: // if null, use this point
            setExtents(point, point);
            return;

        case EXTENT_FINITE:
            mMaximum.makeCeil(point);
            mMinimum.makeFloor(point);
            return;

        case EXTENT_INFINITE: // if infinite, makes no difference
            return;
        }

        BASSERT( false && "Never reached" );
    }

    /** Sets the box to a 'null' value i.e. not a box.
    */
    inline void setNull()
    {
        mExtent = EXTENT_NULL;
    }

    /** Returns true if the box is null i.e. empty.
    */
    inline bool isNull(void) const
    {
        return (mExtent == EXTENT_NULL);
    }

    /** Returns true if the box is finite.
    */
    bool isFinite(void) const
    {
        return (mExtent == EXTENT_FINITE);
    }

    /** Sets the box to 'infinite'
    */
    inline void setInfinite()
    {
        mExtent = EXTENT_INFINITE;
    }

    /** Returns true if the box is infinite.
    */
    bool isInfinite(void) const
    {
        return (mExtent == EXTENT_INFINITE);
    }

    /** Returns whether or not this box intersects another. */
    inline bool intersects(const axisAlignedBox& b2) const
    {
        // Early-fail for nulls
        if (this->isNull() || b2.isNull())
            return false;

        // Early-success for infinites
        if (this->isInfinite() || b2.isInfinite())
            return true;

        // Use up to 6 separating planes
        if (mMaximum.x < b2.mMinimum.x)
            return false;
        if (mMaximum.y < b2.mMinimum.y)
            return false;
        if (mMaximum.z < b2.mMinimum.z)
            return false;

        if (mMinimum.x > b2.mMaximum.x)
            return false;
        if (mMinimum.y > b2.mMaximum.y)
            return false;
        if (mMinimum.z > b2.mMaximum.z)
            return false;

        // otherwise, must be intersecting
        return true;

    }

    /// Calculate the area of intersection of this box and another
    inline axisAlignedBox intersection(const axisAlignedBox& b2) const
    {
        if (this->isNull() || b2.isNull())
        {
            return axisAlignedBox();
        }
        else if (this->isInfinite())
        {
            return b2;
        }
        else if (b2.isInfinite())
        {
            return *this;
        }

        vector3 intMin = mMinimum;
        vector3 intMax = mMaximum;

        intMin.makeCeil(b2.getMinimum());
        intMax.makeFloor(b2.getMaximum());

        // Check intersection isn't null
        if (intMin.x < intMax.x &&
            intMin.y < intMax.y &&
            intMin.z < intMax.z)
        {
            return axisAlignedBox(intMin, intMax);
        }

        return axisAlignedBox();
    }

    /// Calculate the volume of this box
    real volume(void) const;

    /** Scales the AABB by the vector given. */
    inline void scale(const vector3& s)
    {
        // Do nothing if current null or infinite
        if (mExtent != EXTENT_FINITE)
            return;

        // NB assumes centered on origin
        vector3 min = mMinimum * s;
        vector3 max = mMaximum * s;
        setExtents(min, max);
    }

    /** Tests whether this box intersects a sphere. */
    bool intersects(const sphere& s) const;

    /** Tests whether this box intersects a plane. */
    bool intersects(const plane& p) const;

    /** Tests whether the vector point is within this box. */
    bool intersects(const vector3& v) const
    {
        switch (mExtent)
        {
        case EXTENT_NULL:
            return false;

        case EXTENT_FINITE:
            return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  &&
                v.y >= mMinimum.y  &&  v.y <= mMaximum.y  &&
                v.z >= mMinimum.z  &&  v.z <= mMaximum.z);

        case EXTENT_INFINITE:
            return true;

        default: // shut up compiler
            BASSERT( false && "Never reached" );
            return false;
        }
    }
    /// Gets the centre of the box
    vector3 getCenter(void) const
    {
        BASSERT( (mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB" );

        return vector3(
            (mMaximum.x + mMinimum.x) * 0.5f,
            (mMaximum.y + mMinimum.y) * 0.5f,
            (mMaximum.z + mMinimum.z) * 0.5f);
    }
    /// Gets the size of the box
    vector3 getSize(void) const;

    /// Gets the half-size of the box
    vector3 getHalfSize(void) const;

    /** Tests whether the given point contained by this box.
    */
    bool contains(const vector3& v) const
    {
        if (isNull())
            return false;
        if (isInfinite())
            return true;

        return mMinimum.x <= v.x && v.x <= mMaximum.x &&
               mMinimum.y <= v.y && v.y <= mMaximum.y &&
               mMinimum.z <= v.z && v.z <= mMaximum.z;
    }

    /** Returns the minimum distance between a given point and any part of the box. */
    real distance(const vector3& v) const;

    /** Tests whether another box contained by this box.
    */
    bool contains(const axisAlignedBox& other) const
    {
        if (other.isNull() || this->isInfinite())
            return true;

        if (this->isNull() || other.isInfinite())
            return false;

        return this->mMinimum.x <= other.mMinimum.x &&
               this->mMinimum.y <= other.mMinimum.y &&
               this->mMinimum.z <= other.mMinimum.z &&
               other.mMaximum.x <= this->mMaximum.x &&
               other.mMaximum.y <= this->mMaximum.y &&
               other.mMaximum.z <= this->mMaximum.z;
    }

    /** Tests 2 boxes for equality.
    */
    bool operator== (const axisAlignedBox& rhs) const
    {
        if (this->mExtent != rhs.mExtent)
            return false;

        if (!this->isFinite())
            return true;

        return this->mMinimum == rhs.mMinimum &&
               this->mMaximum == rhs.mMaximum;
    }

    /** Tests 2 boxes for inequality.
    */
    bool operator!= (const axisAlignedBox& rhs) const
    {
        return !(*this == rhs);
    }

    // special values
    static const axisAlignedBox BOX_NULL;
    static const axisAlignedBox BOX_INFINITE;


    axisAlignedBox move(const vector3& vec) const {return axisAlignedBox(getMinimum()+vec, getMaximum()+vec);}

    axisAlignedBox operator * (const real& mul) const
    {
        return axisAlignedBox(getMinimum()*mul, getMaximum()*mul);
    }
    axisAlignedBox& operator *= (const real& mul)
    {
        mMinimum*=mul;
        mMaximum*=mul;
        return *this;
    }
    axisAlignedBox operator / (const real& mul) const
    {
        return axisAlignedBox(getMinimum()/mul, getMaximum()/mul);
    }

    void setMinimumAndMaximum(const vector3& min, const vector3& max)
    {
        setMinimum(min);
        setMaximum(max);
    }

private:
    BLUB_SERIALIZATION_ACCESS

    template <class formatType>
    void save(formatType & readWrite, const uint32& version) const
    {
        (void)version;

        const vector3 minumum(mMinimum);
        const vector3 maximum(mMaximum);
        readWrite & serialization::nameValuePair::create("minimum", minumum);
        readWrite & serialization::nameValuePair::create("maximum", maximum);
        readWrite & serialization::nameValuePair::create("extend", mExtent);
    }
    template <class formatType>
    void load(formatType & readWrite, const uint32& version)
    {
        (void)version;

        vector3 minumum;
        vector3 maximum;
        readWrite & serialization::nameValuePair::create("minimum", minumum);
        readWrite & serialization::nameValuePair::create("maximum", maximum);
        setMinimumAndMaximum(minumum, maximum);
        readWrite & serialization::nameValuePair::create("extend", mExtent);
    }
    BLUB_SERIALIZATION_SAVELOAD()

};

std::ostream& operator<< (std::ostream& ostr, const axisAlignedBox& toCast);


}
BLUB_CLASSVERSION(blub::axisAlignedBox, 1)

#endif // AXISALIGNEDBOX_HPP
