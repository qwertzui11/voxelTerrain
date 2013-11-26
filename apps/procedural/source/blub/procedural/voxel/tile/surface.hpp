#ifndef PROCEDURAL_VOXEL_TILE_SURFACE_HPP
#define PROCEDURAL_VOXEL_TILE_SURFACE_HPP

#include "blub/core/sharedPointer.hpp"
#include "blub/core/vector.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


class surface : public base<surface>
{
public:
    typedef base<surface> t_base;
    typedef sharedPointer<accessor> t_voxelAccessorPtr;
    typedef vector<vector3> t_positionsList;
    typedef vector<vector3> t_normalsList;
    typedef vector<uint16> t_indicesList;

    static pointer create();
    ~surface();

    void calculateSurface(const t_voxelAccessorPtr voxel,
                          const vector3& vertexOffset = vector3(), const real &voxelSize = 1.,
                          const bool& calculateNornalCorrection = true,
                          const int32 &lod = 0);
    void clear();

    bool getCaluculateTransvoxel(void) const;
    bool getCaluculateLod(void) const;

    const t_positionsList& getPositions(void) const;
    const t_normalsList& getNormals(void) const;
    const t_indicesList& getIndices(void) const;
    const t_indicesList& getIndicesLod(const uint16& lod) const;

protected:
    surface();

private:
    int8 getVoxelInterpolation(const vector3int32& pos);
    int8 getVoxelInterpolationLod(const vector3int32& pos, const uint16 &lod);
    int32 calculateEdgeId(const vector3int32& pos, const int32& edgeInformation) const;
    int32 calculateEdgeIdTransvoxel(const vector3int32& pos, const int32& edgeInformation, const int32& coord) const;
    int32 calculateVertexId(const vector3int32& pos) const;
    int32 calculateVertexIdTransvoxel(const vector2int32& pos) const;
    vector3 calculateIntersectionPosition(const vector3int32& pos, const int32& corner0, const int32& corner1);
    vector3 calculateIntersectionPositionTransvoxel(const vector3int32& pos, const int32& corner0, const int32& corner1, const vector3int32 voxel[], const uint16 &lod);
    static vector3int32 calculateCorner(const int32& corner);
    static vector3int32 calculateCornerTransvoxel(const int32& corner, const vector3int32 voxel[]);
    static vector3 getInterpolatedPosition(const vector3& positionFrom, const vector3& positionTo, const int8& interpolationFrom, const int8& interpolationTo);

private:
    t_voxelAccessorPtr m_voxel;
    int32 m_lod;
    int32 m_voxelSkip;

    t_positionsList m_positions;
    t_normalsList m_normals;
    t_indicesList m_indices;
    t_indicesList m_indicesLod[6];
};


}
}
}
}


#endif // PROCEDURAL_VOXEL_TILE_SURFACE_HPP
