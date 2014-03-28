#ifndef BLUB_PROCEDURAL_VOXEL_EDIT_NOISE_HPP
#define BLUB_PROCEDURAL_VOXEL_EDIT_NOISE_HPP

#include "blub/core/vector.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/procedural/log/global.hpp"
#include "blub/procedural/voxel/edit/base.hpp"

#include <boost/function/function2.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


/**
 * @brief The noise class generates a random terrain using simplex noise http://en.wikipedia.org/wiki/Simplex_noise .
 * Original code http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java
 */
template <class voxelType>
class noise : public base<voxelType>
{
public:
    typedef boost::function<bool (vector3, real&)> t_callbackInterpolation;

    typedef base<voxelType> t_base;
    typedef sharedPointer<noise> pointer;

    /**
     * @brief creates an instance of the class and returns it as shared_ptr<>
     * @param desc Defines the size in which the terrain should get generated.
     * @param scale Gets used to scale the voxel-position before calculating the interpolation
     * @param seed Used before calling std::random_shuffle
     * @param callbackInterpolation Callback for the generated interpolation.
     * @return An instance of the class as shared_ptr<>
     */
    static pointer create(const blub::axisAlignedBox& desc,
                          const vector3& scale,
                          const uint32& seed = 0,
                          const t_callbackInterpolation& callbackInterpolation = [] (const vector3&, real& value) {value*=1024;return true;})
    {
        return pointer(new noise(desc, scale, seed, callbackInterpolation));
    }
    /**
     * @brief ~noise destructor
     */
    virtual ~noise()
    {
        ;
    }

    /**
     * @brief getAxisAlignedBoundingBox returns transformed aab of the to generate voxel
     * @param trans Transform
     * @return
     */
    blub::axisAlignedBox getAxisAlignedBoundingBox(const transform& trans) const override
    {
        return blub::axisAlignedBox(m_aab.getMinimum() + trans.position,
                                    m_aab.getMaximum() + trans.position);
    }

protected:
    static real fade(const real &t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    static real lerp(const real& t, const real& a, const real& b)
    {
        return a + t * (b - a);
    }
    static real grad(const int32& hash, const real& x, const real& y, const real& z) {
        const int32 h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
        const real u = h < 8 ? x : y,                   // INTO 12 GRADIENT DIRECTIONS.
                   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    int32 permutation(const int32 index) const
    {
        BASSERT(index >= 0);
        BASSERT(index < 512);
        return m_permutation[index%256];
    }

    /**
     * @brief calculateOneVoxel scales pos by scale set in constructor and calculates the noise.
     * @param pos absolute voxel-position
     * @param resultVoxel gets set if interpolation larger -127
     * @return Returns true if interpolation larger -127
     */
    bool calculateOneVoxel(const vector3& pos, voxelType* resultVoxel) const override
    {
        real x(pos.x*m_scale.x);
        real y(pos.y*m_scale.y);
        real z(pos.z*m_scale.z);

        const int32 X = static_cast<int32>(math::floor(x)) & 255;                  // FIND UNIT CUBE THAT
        const int32 Y = static_cast<int32>(math::floor(y)) & 255;                  // CONTAINS POINT.
        const int32 Z = static_cast<int32>(math::floor(z)) & 255;
        x -= static_cast<int32>(math::floor(x));                                // FIND RELATIVE X,Y,Z
        y -= static_cast<int32>(math::floor(y));                                // OF POINT IN CUBE.
        z -= static_cast<int32>(math::floor(z));
        const real u = fade(x);                                // COMPUTE FADE CURVES
        const real v = fade(y);                                // FOR EACH OF X,Y,Z.
        const real w = fade(z);
        const int32 A =  permutation(X  )+Y;
        const int32 AA = permutation(A)+Z;
        const int32 AB = permutation(A+1)+Z;      // HASH COORDINATES OF
        const int32 B =  permutation(X+1)+Y;
        const int32 BA = permutation(B)+Z;
        const int32 BB = permutation(B+1)+Z;      // THE 8 CUBE CORNERS,

        real resultInterpolation =   lerp(w, lerp(v, lerp(u, grad(permutation(AA  ), x  , y  , z   ),  // AND ADD
                                                             grad(permutation(BA  ), x-1, y  , z   )), // BLENDED
                                                     lerp(u, grad(permutation(AB  ), x  , y-1, z   ),  // RESULTS
                                                             grad(permutation(BB  ), x-1, y-1, z   ))),// FROM  8
                                             lerp(v, lerp(u, grad(permutation(AA+1), x  , y  , z-1 ),  // CORNERS
                                                             grad(permutation(BA+1), x-1, y  , z-1 )), // OF CUBE
                                                     lerp(u, grad(permutation(AB+1), x  , y-1, z-1 ),
                                                             grad(permutation(BB+1), x-1, y-1, z-1 ))));
        if (!m_callbackInterpolation(pos, resultInterpolation))
        {
            return false;
        }
        const int8 resultCasted(static_cast<int8>(math::clamp<real>(resultInterpolation, -127., 127.)));

        resultVoxel->setInterpolation(resultCasted);

        return true;
    }


    /**
     * @brief noise Contructor - same as create()
     */
    noise(const blub::axisAlignedBox& desc, const vector3& scale, const uint32& seed, const t_callbackInterpolation &callbackInterpolation)
        : m_aab(desc)
        , m_scale(scale)
        , m_callbackInterpolation(callbackInterpolation)
        , m_permutation(256)
    {
        std::srand(seed);
        std::iota(m_permutation.begin(), m_permutation.end(), 0);
        std::random_shuffle(m_permutation.begin(), m_permutation.end());
    }

protected:
    const blub::axisAlignedBox m_aab;
    const vector3 m_scale;
    const t_callbackInterpolation m_callbackInterpolation;

    vector<uint8_t> m_permutation;

};


}
}
}
}


#endif // BLUB_PROCEDURAL_VOXEL_EDIT_NOISE_HPP
