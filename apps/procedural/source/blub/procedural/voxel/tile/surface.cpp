#include "surface.hpp"

#include "blub/math/axisAlignedBox.hpp"
#include "blub/core/log.hpp"
#include "blub/math/math.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/math/vector2int32.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/accessor.hpp"
#include "blub/procedural/voxel/tile/internal/transvoxelTables.hpp"


using namespace blub::procedural::voxel::tile;
using namespace blub::procedural::voxel;
using namespace blub;


surface::surface()
{
}

surface::t_base::pointer surface::create()
{
    return new surface();
}

surface::~surface()
{
#ifdef BLUB_LOG_VOXEL_SURFACE
    blub::BOUT("surface::~surface()");
#endif
}



void surface::calculateSurface(const surface::t_voxelAccessorPtr voxel,
                            const vector3 &vertexOffset,
                            const real &voxelSize,
                            const bool &calculateNormalCorrection,
                            const int32 &lod)
{
#ifdef BLUB_LOG_VOXEL_SURFACE
    blub::BOUT("surface::calculateSurface(..) lod:" + blub::string::number(lod));
#endif

    clear();

    m_voxel = voxel;
    m_lod = lod;
    m_voxelSkip = math::pow(2, m_lod);

    m_positions.reserve(1000);
    m_normals.reserve(1000);
    m_indices.reserve(2000);

    const vector3int32 voxelStart(-1);
    const vector3int32 voxelEnd(tile::accessor::voxelLength+2);

    const int32 vertexIndicesReuseSize(((voxelEnd.x-voxelStart.x)*3)*((voxelEnd.y-voxelStart.y)*3)*((voxelEnd.z-voxelStart.z)*3));
        // the indexer for the vertices. *3 because gets saved with edge-id
    int32 *vertexIndicesReuse = new int32[vertexIndicesReuseSize];
    for (int32 index = 0; index < vertexIndicesReuseSize; ++index)
    {
        vertexIndicesReuse[index] = -1;
    }

    // isLevel describes at which interpolation-level a surface is generated around the voxel
    const int8 isoLevel(0);
    for (int32 x = voxelStart.x; x < voxelEnd.x-1; ++x)
    {
        for (int32 y = voxelStart.y; y < voxelEnd.y-1; ++y)
        {
            for (int32 z = voxelStart.z; z < voxelEnd.z-1; ++z)
            {
                // depending on the voxel-neighbour- the count and look, of the triangles gets calculated.
                uint8 tableIndex(0);
                const vector3int32 posVoxel(x, y, z);
                if (getVoxelInterpolation(posVoxel) < isoLevel) // <-- getVoxelInterpolation needs 85% of calculationTime;
                    tableIndex |= 1;
                if (getVoxelInterpolation(posVoxel + vector3int32(1, 0, 0)) < isoLevel)
                    tableIndex |= 2;
                if (getVoxelInterpolation(posVoxel + vector3int32(0, 0, 1)) < isoLevel)
                    tableIndex |= 4;
                if (getVoxelInterpolation(posVoxel + vector3int32(1, 0, 1)) < isoLevel)
                    tableIndex |= 8;
                if (getVoxelInterpolation(posVoxel + vector3int32(0, 1, 0)) < isoLevel)
                    tableIndex |= 16;
                if (getVoxelInterpolation(posVoxel + vector3int32(1, 1, 0)) < isoLevel)
                    tableIndex |= 32;
                if (getVoxelInterpolation(posVoxel + vector3int32(0, 1, 1)) < isoLevel)
                    tableIndex |= 64;
                if (getVoxelInterpolation(posVoxel + vector3int32(1, 1, 1)) < isoLevel)
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if (tableIndex != 0 && tableIndex != 255)
                {
                    bool calculateFaces(true);
                    if (calculateNormalCorrection)
                    {
                        calculateFaces = (posVoxel >= vector3int32(0) && posVoxel < voxelEnd - vector3int32(2));
                    }
                    // OPTIMISE: too many vertices get calculated because of normalcorrection; optimise!
                    const RegularCellData *data = &regularCellData[regularCellClass[tableIndex]];
                    int32 ids[12];
                    for (int32 ind = 0; ind < data->GetVertexCount(); ++ind)
                    {
                        int32 data2 = regularVertexData[tableIndex][ind];
                        int32 corner0 = data2 & 0x0F;
                        int32 corner1 = (data2 & 0xF0) >> 4;
                        int32 id = calculateEdgeId(posVoxel, data2 >> 8); // for reuse
                        BASSERT(id >= 0);
                        BASSERT(id < vertexIndicesReuseSize);

                        if (vertexIndicesReuse[id] == -1)
                        {
                            vector3 point = calculateIntersectionPosition(posVoxel, corner0, corner1);
                            // minus 1 because of normal-correction-offset 1
                            // we calucluate here everything in positive values; but normal correction starts @ -1
                            point *= voxelSize;
                            point += vertexOffset;
                            // point += vertexOffset/30.;
                            m_positions.push_back(point);
                            m_normals.push_back(vector3());
                            BASSERT(m_positions.size() == m_normals.size());
                            ids[ind] = vertexIndicesReuse[id] = m_positions.size()-1;
                        }
                        else
                        {
                            ids[ind] = vertexIndicesReuse[id];
                        }
                    }
                    for (int32 ind = 0; ind < data->GetTriangleCount()*3; ind+=3)
                    {
                        // calc normal
                        const int32 vertexIndex0(ids[data->vertexIndex[ind+0]]);
                        const int32 vertexIndex1(ids[data->vertexIndex[ind+1]]);
                        const int32 vertexIndex2(ids[data->vertexIndex[ind+2]]);
                        const vector3 vertex0(m_positions.at(vertexIndex0));
                        const vector3 vertex1(m_positions.at(vertexIndex1));
                        const vector3 vertex2(m_positions.at(vertexIndex2));
                        if (vertex0 == vertex1 || vertex1 == vertex2 || vertex0 == vertex2)
                        {
                            continue; // triangle with zero space
                        }
                        const vector3 addNormal = (vertex1 - vertex0).crossProduct(vertex2 - vertex0);
                        // if (calculateFaces) // for normal-correction-test
                        {
                            m_normals.at(vertexIndex0) += addNormal;
                            m_normals.at(vertexIndex1) += addNormal;
                            m_normals.at(vertexIndex2) += addNormal;
                        }
                        if (calculateFaces)
                        {
                            // insert new triangle
                            m_indices.push_back(vertexIndex0);
                            m_indices.push_back(vertexIndex1);
                            m_indices.push_back(vertexIndex2);
                        }
                    }
                }
            }
        }
    }

    // transvoxel
    if (m_lod > 0)// && false)
    {
        const vector3int32 voxelLookups[][9] = {
            {{0, 0, 0},{0, 1, 0},{0, 2, 0},{0, 2, 1},{0, 2, 2},{0, 1, 2},{0, 0, 2},{0, 0, 1},{0, 1, 1}},
            {{0, 0, 0},{1, 0, 0},{2, 0, 0},{2, 0, 1},{2, 0, 2},{1, 0, 2},{0, 0, 2},{0, 0, 1},{1, 0, 1}},
            {{0, 0, 0},{1, 0, 0},{2, 0, 0},{2, 1, 0},{2, 2, 0},{1, 2, 0},{0, 2, 0},{0, 1, 0},{1, 1, 0}},
            };
        const int32 voxelLengthLodStart(accessor::voxelLengthLod-2);
        const int32 voxelLengthLodEnd(accessor::voxelLengthLod-1);
        const vector3int32 toIterate[][2] = {
            {{0, 0, 0},                     {1, voxelLengthLodStart, voxelLengthLodStart}},
            {{voxelLengthLodStart, 0, 0},   {voxelLengthLodEnd, voxelLengthLodStart, voxelLengthLodStart}},
            {{0, 0, 0},                     {voxelLengthLodStart, 1, voxelLengthLodStart}},
            {{0, voxelLengthLodStart, 0},   {voxelLengthLodStart, voxelLengthLodEnd, voxelLengthLodStart}},
            {{0, 0, 0},                     {voxelLengthLodStart, voxelLengthLodStart, 1}},
            {{0, 0, voxelLengthLodStart},   {voxelLengthLodStart, voxelLengthLodStart, voxelLengthLodEnd}}
            };
        const vector3int32 reuseCorrection[] = {
            {1, 0, 0},
            {1, 0, 0},
            {0, 1, 0},
            {0, 1, 0},
            {0, 0, 1},
            {0, 0, 1}
            };

        const bool toInvertTriangles[] = {
            false, true,
            true, false, // data from Eric Lengyel seems to have different axis-desc
            false, true
            };

        for (int32 lod = 0; lod < 6; ++lod)
        {
            const int32 coord(lod/2);
            const vector3int32& start(toIterate[lod][0]);
            const vector3int32& end  (toIterate[lod][1]);
            const bool invertTriangles(toInvertTriangles[lod]);

            // the indexer for the vertices. *3 because gets saved with edge-id
            const int32 vertexIndicesReuseLodSize(((accessor::voxelLength+1)*4)*
                                                  ((accessor::voxelLength+1)*4));
            int32 vertexIndicesReuseLod[vertexIndicesReuseLodSize];
            for (int32 index = 0; index < vertexIndicesReuseLodSize; ++index)
            {
                vertexIndicesReuseLod[index] = -1;
            }

            for (uint32 x = start.x; x < (unsigned)end.x; x+=2)
            {
                for (uint32 y = start.y; y < (unsigned)end.y; y+=2)
                {
                    for (uint32 z = start.z; z < (unsigned)end.z; z+=2)
                    {
                        const vector3int32 voxelPos(x, y, z);
                        {
                            uint32 tableIndex(0);
                            uint32 add(1);
                            for (uint16 ind = 0; ind < 9; ++ind)
                            {
                                const vector3int32 lookUp((voxelPos-start)+voxelLookups[coord][ind]);
                                if (getVoxelInterpolationLod(lookUp, lod) < isoLevel)
                                {
                                    tableIndex |= add;
                                }
                                add*=2;
                            }
                            if (tableIndex == 0 || tableIndex == 511) // no triangles
                            {
                                continue;
                            }

                            uint32 classIndex = transitionCellClass[tableIndex];
                            const TransitionCellData *data = &transitionCellData[classIndex & 0x7F]; // only the last 7 bit count
                            uint32 ids[12];

                            vector3 normalsForTransvoxel[4];

                            for (uint16 ind = 0; ind < data->GetVertexCount(); ++ind)
                            {
                                const uint16 data2 = transitionVertexData[tableIndex][ind];
                                const uint16 edge = data2 >> 8;
                                const uint16 edgeId = edge & 0x0F;
                                const uint16 edgeBetween(data2 & 0xFF);

                                if (edgeId == 0x9 || edgeId == 0x8)
                                {
                                    BASSERT(edge == 0x88 || edge == 0x28 || edge == 0x89 || edge == 0x19);

                                    const uint16 owner((edge & 0xF0) >> 4);
                                    BASSERT(owner == 1 || owner == 2 || owner == 8);

                                    uint16 newEdgeId(0);
                                    uint16 newOwner(0);

                                    if (coord == 0)
                                    {
                                        if (edgeId == 0x8)
                                        {
                                            newEdgeId = 0x3;
                                        }
                                        else
                                        {
                                            newEdgeId = 0x1;
                                        }
                                        if (owner == 0x1)
                                        {
                                            newOwner = 0x4;
                                        }
                                        if (owner == 0x2)
                                        {
                                            newOwner = 0x2;
                                        }
                                    }
                                    if (coord == 1)
                                    {
                                        if (edgeId == 0x8)
                                        {
                                            newEdgeId = 0x2;
                                        }
                                        else
                                        {
                                            newEdgeId = 0x1;
                                        }
                                        newOwner = owner;
                                    }
                                    if (coord == 2)
                                    {
                                        newEdgeId = edgeId - 6;
                                        if (owner == 0x1)
                                        {
                                            newOwner = 0x1;
                                        }
                                        if (owner == 0x2)
                                        {
                                            newOwner = 0x4;
                                        }
                                    }


                                    uint16 newEdge((newOwner << 4) | newEdgeId);
                                    const int32 id = calculateEdgeId((voxelPos / 2) - reuseCorrection[lod], newEdge);

                                    BASSERT(vertexIndicesReuse[id] != -1);

                                    ids[ind]=vertexIndicesReuse[id];

                                    const vector3& normal(m_normals.at(ids[ind]));
                                    switch (edgeBetween)
                                    {
                                    case 0x9A:
                                        normalsForTransvoxel[0] = normal;
                                        break;
                                    case 0xAC:
                                        normalsForTransvoxel[1] = normal;
                                        break;
                                    case 0xBC:
                                        normalsForTransvoxel[2] = normal;
                                        break;
                                    case 0x9B:
                                        normalsForTransvoxel[3] = normal;
                                        break;
                                    default:
                                        BASSERT(false);
                                    }
                                }
                            }
                            for (uint16 ind = 0; ind < data->GetVertexCount(); ++ind)
                            {
                                const uint16 data2 = transitionVertexData[tableIndex][ind];
                                const uint16 edge = data2 >> 8;
                                const uint16 edgeId = edge & 0x0F;
                                const uint16 edgeBetween(data2 & 0xFF);

                                if (edgeId != 0x9 && edgeId != 0x8)
                                {
                                    const uint16 corner0 = data2 & 0x0F;
                                    const uint16 corner1 = (data2 & 0xF0) >> 4;


                                    const int32 id = calculateEdgeIdTransvoxel(voxelPos/2, edge, coord);

                                    //blub::BOUT("edge:" + blub::string::number(edge, 16) + " id:" + blub::string::number(id));

                                    bool calculateVertexPosition(id == -1);
                                    if (!calculateVertexPosition)
                                    {
                                        calculateVertexPosition = vertexIndicesReuseLod[id] == -1;
                                    }

                                    if (calculateVertexPosition)
                                    {
                                        vector3 point = calculateIntersectionPositionTransvoxel(voxelPos-start, corner0, corner1, voxelLookups[coord], lod);
                                        point+=vector3(start)/2.;

                                        point *= voxelSize;
                                        point += vertexOffset;

                                        //blub::BOUT("doing new transvoxel-vertex: resultPosition:" + blub::string::number(point));

                                        m_positions.push_back(point);
                                        switch (edgeBetween)
                                        {
                                        case 0x01:
                                        case 0x12:
                                            m_normals.push_back(normalsForTransvoxel[0]);
                                            break;
                                        case 0x03:
                                        case 0x36:
                                            m_normals.push_back(normalsForTransvoxel[3]);
                                            break;
                                        case 0x25:
                                        case 0x58:
                                            m_normals.push_back(normalsForTransvoxel[1]);
                                            break;
                                        case 0x67:
                                        case 0x78:
                                            m_normals.push_back(normalsForTransvoxel[2]);
                                            break;
                                        case 0x34:
                                        case 0x14:
                                        case 0x45:
                                        case 0x47:
                                            m_normals.push_back(normalsForTransvoxel[0] + normalsForTransvoxel[1] + normalsForTransvoxel[2] + normalsForTransvoxel[3]);
                                            break;
                                        default:
                                            BASSERT(false);
                                        }
                                        BASSERT(m_positions.size() == m_normals.size());

                                        if (id == -1)
                                        {
                                            ids[ind] = m_positions.size()-1;
                                        }
                                        else
                                        {
                                            ids[ind] = vertexIndicesReuseLod[id] = m_positions.size()-1;
                                        }
                                    }
                                    else
                                    {
                                        ids[ind] = vertexIndicesReuseLod[id];
                                        //blub::BOUT("doing transvoxel-reuse: resultPosition:" + blub::string::number((m_positions.at(ids[ind]))));
                                    }
                                }
                            }
                            for (uint16 ind = 0; ind < data->GetTriangleCount()*3; ind+=3)
                            {
                                // calc normal
                                const int32 vertexIndex0(ids[data->vertexIndex[ind+0]]);
                                const int32 vertexIndex1(ids[data->vertexIndex[ind+1]]);
                                const int32 vertexIndex2(ids[data->vertexIndex[ind+2]]);
                                const vector3 vertex0(m_positions.at(vertexIndex0));
                                const vector3 vertex1(m_positions.at(vertexIndex1));
                                const vector3 vertex2(m_positions.at(vertexIndex2));
                                if (vertex0 == vertex1 || vertex1 == vertex2 || vertex0 == vertex2)
                                {
                                    continue; // triangle with zero space
                                }
                                // insert new triangle
                                uint16 invert(1);
                                if (invertTriangles)
                                {
                                    invert = 0;
                                }
                                m_indicesLod[lod].push_back(vertexIndex0);
                                if ((classIndex >> 7) % 2 == invert)
                                {
                                    m_indicesLod[lod].push_back(vertexIndex1);
                                    m_indicesLod[lod].push_back(vertexIndex2);
                                }
                                else
                                {
                                    m_indicesLod[lod].push_back(vertexIndex2);
                                    m_indicesLod[lod].push_back(vertexIndex1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    delete [] vertexIndicesReuse;

    // normalise normals
    for (uint32 ind = 0; ind < m_normals.size(); ++ind)
    {
        m_normals.at(ind).normalise();
    }

#ifdef BLUB_LOG_VOXEL_SURFACE
    blub::BOUT("surface::calculateSurface(..) end");
#endif
}

void surface::clear()
{
    m_positions.clear();
    m_normals.clear();
    m_indices.clear();
    for (int32 lod = 0; lod < 6; ++lod)
    {
        m_indicesLod[lod].clear();
    }
}


bool surface::getCaluculateTransvoxel() const
{
    return m_lod > 0;
}

bool surface::getCaluculateLod() const
{
    return m_lod > 0;
}

const surface::t_positionsList &surface::getPositions() const
{
    return m_positions;
}

const surface::t_normalsList &surface::getNormals() const
{
    return m_normals;
}

const surface::t_indicesList &surface::getIndices() const
{
    return m_indices;
}

const surface::t_indicesList &surface::getIndicesLod(const uint16 &lod) const
{
    BASSERT(lod < 6);
    return m_indicesLod[lod];
}

int8 surface::getVoxelInterpolation(const vector3int32 &pos)
{   
    return m_voxel->getVoxel(pos).interpolation;
}

int8 surface::getVoxelInterpolationLod(const vector3int32 &pos, const uint16& lod)
{
    return m_voxel->getVoxelLod(pos, lod).interpolation;
}

int32 surface::calculateEdgeId(const vector3int32 &pos, const int32 &edgeInformation) const
{
    const int32 edge(edgeInformation & 0x0F);
    const int32 owner((edgeInformation & 0xF0) >> 4);
    const int32 diffX(owner % 2);
    const int32 diffY((owner >> 2) % 2); // !!! order
    const int32 diffZ((owner >> 1) % 2);

    BASSERT((diffX == 0) || (diffX == 1));
    BASSERT((diffY == 0) || (diffY == 1));
    BASSERT((diffZ == 0) || (diffZ == 1));

    return calculateVertexId(pos - vector3int32(diffX, diffY, diffZ)) + (edge-1);
}

int32 surface::calculateEdgeIdTransvoxel(const vector3int32 &pos, const int32 &edgeInformation, const int32& coord) const
{
    const int32 edge(edgeInformation & 0x0F);
    const int32 owner((edgeInformation & 0xF0) >> 4);
    const int32 diffFst(owner % 2);
    const int32 diffSnd((owner >> 1) % 2);

    BASSERT(edge >= 3);
    BASSERT(edge <= 6);
    BASSERT(owner == 1 || owner == 2 || owner == 4 || owner == 8);
    BASSERT((diffFst == 0) || (diffFst == 1));
    BASSERT((diffSnd == 0) || (diffSnd == 1));

    if (owner == 4) // no reuse
    {
        return -1;
    }
    switch (coord) {
    case 0:
        return calculateVertexIdTransvoxel(vector2int32(pos.y - diffFst, pos.z - diffSnd)) + (edge-3);
    case 1:
        return calculateVertexIdTransvoxel(vector2int32(pos.x - diffFst, pos.z - diffSnd)) + (edge-3);
    case 2:
        return calculateVertexIdTransvoxel(vector2int32(pos.x - diffFst, pos.y - diffSnd)) + (edge-3);
    default:
        break;
    }

    BASSERT(false);
    return -1;
}

int32 surface::calculateVertexId(const vector3int32 &pos) const
{
    return (
            ((pos.x+2)*3) * ((accessor::voxelLengthWithNormalCorrection)*3)*((accessor::voxelLengthWithNormalCorrection)*3) +
            ((pos.y+2)*3) * ((accessor::voxelLengthWithNormalCorrection)*3) +
            ((pos.z+2)*3)
            );
}

int32 surface::calculateVertexIdTransvoxel(const vector2int32 &pos) const
{
    return (pos.x+1)*4*(accessor::voxelLength+1) +
           (pos.y+1)*4;
}


vector3 surface::calculateIntersectionPosition(const vector3int32 &pos, const int32 &corner0, const int32 &corner1)
{
    const vector3int32 corn0(calculateCorner(corner0));
    const vector3int32 corn1(calculateCorner(corner1));

    const int8 interpolation0 = getVoxelInterpolation(corn0+pos);
    const int8 interpolation1 = getVoxelInterpolation(corn1+pos);

    const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

    return vector3(pos) + result;
}

vector3 surface::calculateIntersectionPositionTransvoxel(const vector3int32 &pos,
                                                      const int32 &corner0,
                                                      const int32 &corner1,
                                                      const vector3int32 voxel[9],
                                                      const uint16& lod)
{
    const vector3int32 corn0(calculateCornerTransvoxel(corner0, voxel));
    const vector3int32 corn1(calculateCornerTransvoxel(corner1, voxel));

    const int8 interpolation0 = getVoxelInterpolationLod(corn0+pos, lod);
    const int8 interpolation1 = getVoxelInterpolationLod(corn1+pos, lod);

    const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

    return (vector3(pos) + result)/2.;
}

vector3int32 surface::calculateCorner(const int32 &corner)
{
    switch (corner)
    {
    case 0:
        return vector3int32(0, 0, 0);
    case 1:
        return vector3int32(1, 0, 0);
    case 2:
        return vector3int32(0, 0, 1);
    case 3:
        return vector3int32(1, 0, 1);
    case 4:
        return vector3int32(0, 1, 0);
    case 5:
        return vector3int32(1, 1, 0);
    case 6:
        return vector3int32(0, 1, 1);
    case 7:
        return vector3int32(1, 1, 1);
    default:
        // fatal
        break;
    }
    // never reach!
    BASSERT(false);

    return vector3int32(0, 0, 0); // to remove the compile warning
}

vector3int32 surface::calculateCornerTransvoxel(const int32 &corner, const vector3int32 voxel[9])
{
    switch (corner)
    {
    case 0x0:
        return vector3int32(0, 0, 0);
    case 0x1:
        return voxel[1];
    case 0x2:
        return voxel[2];
    case 0x3:
        return voxel[7];
    case 0x4:
        return voxel[8];
    case 0x5:
        return voxel[3];
    case 0x6:
        return voxel[6];
    case 0x7:
        return voxel[5];
    case 0x8:
        return voxel[4];
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
    default:
        break;
    }
    BASSERT(false);
    return vector3(0.);
}

vector3 surface::getInterpolatedPosition(const vector3 &positionFrom, const vector3 &positionTo, const int8 &interpolationFrom, const int8 &interpolationTo)
{
    BASSERT(interpolationTo != interpolationFrom);

    const real mu((0.0 - ((real)interpolationFrom)) / ((real)(interpolationTo - interpolationFrom)));
    const vector3 result = positionFrom + mu*(positionTo-positionFrom);

    return result;
}

