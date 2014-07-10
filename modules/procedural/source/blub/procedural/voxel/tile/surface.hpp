#ifndef PROCEDURAL_VOXEL_TILE_SURFACE_HPP
#define PROCEDURAL_VOXEL_TILE_SURFACE_HPP

#include "blub/core/array.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/vector.hpp"
#include "blub/math/vector2int32.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/vector3int32.hpp"
#include "blub/procedural/voxel/tile/base.hpp"
#include "blub/procedural/voxel/tile/internal/transvoxelTables.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace tile
{


/**
 * @brief The surface class convertes a tile::accessor to an iso-surface. Call this class by one thread at a time.
 * Uses marching cubes and transvoxel for surface-generation.
 * http://www.terathon.com/voxels/
 * To understand this class and algos please read Eric Lengyel’s Dissertation: http://www.terathon.com/lengyel/Lengyel-VoxelTerrain.pdf .
 * If you got a custom voxel, that effects the resulting surface derive this class and reimplement vertexGotCreated() and vertexGotCreatedLod().
 * Private methods are undocumentated because they simply implement the code described in Eric Lengyel’s Dissertation.
 */
template <class voxelType>
class surface : public base<surface<voxelType> >
{
public:
    typedef base<surface<voxelType> > t_base;
    typedef accessor<voxelType> t_voxelAccessor;
    typedef sharedPointer<t_voxelAccessor> t_voxelAccessorPtr;
    typedef vector<vector3> t_positionsList;
    typedef vector<vector3> t_normalsList;
    typedef vector<uint16> t_indicesList;

    typedef array<voxelType, 2*2*2> t_calcVoxel;
    typedef array<voxelType, 3*3*3+2*2> t_calcVoxelLod;

    /**
     * @brief create creates an instance.
     * @return never nullptr.
     */
    static typename t_base::pointer create()
    {
        return new surface();
    }

    /**
     * @brief createCopy copies an instance.
     * @param toCopy
     * @return never nullptr.
     */
    static typename t_base::pointer createCopy(typename t_base::pointer toCopy)
    {
        typename t_base::pointer result(create());
        result->m_voxel = toCopy->m_voxel;
        result->m_lod = toCopy->m_lod;
        result->m_positions = toCopy->m_positions;
        result->m_normals = toCopy->m_normals;
        result->m_indices = toCopy->m_indices;
        for (uint16 ind = 0; ind < 6; ++ind)
        {
            result->m_indicesLod[ind] = toCopy->m_indicesLod[ind];
        }
        return result;
    }
    /**
     * @brief ~surface desctructor
     */
    virtual ~surface()
    {
#ifdef BLUB_LOG_VOXEL_SURFACE
        blub::BOUT("surface::~surface()");
#endif
    }

    /**
     * @brief calculateSurface calculates the iso surface.
     * @param voxel Contains the voxel needed for the surface calculation.
     * @param voxelSize voxel-scale.
     * @param calculateNormalCorrection check chapter 3.3 in Eric Lengyel’s Dissertation.
     * @param lod Lod index starting with 0.
     */
    void calculateSurface(const t_voxelAccessorPtr voxel,
                          const real &voxelSize = 1.,
                          const bool& calculateNormalCorrection = true,
                          const int32 &lod = 0)
    {
#ifdef BLUB_LOG_VOXEL_SURFACE
        blub::BOUT("surface::calculateSurface(..) lod:" + blub::string::number(lod));
#endif

        clear();

        m_voxel = voxel;
        m_lod = lod;

        m_positions.reserve(1000);
        m_normals.reserve(1000);
        m_indices.reserve(2000);

        const vector3int32 voxelStart(-1);
        const vector3int32 voxelEnd(t_voxelAccessor::voxelLength+2);

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
                    t_calcVoxel voxelCalc;
                    const vector3int32 toCheck[] = {
                        {0, 0, 0},
                        {1, 0, 0},
                        {0, 0, 1},
                        {1, 0, 1},
                        {0, 1, 0},
                        {1, 1, 0},
                        {0, 1, 1},
                        {1, 1, 1}
                    };
                    uint16 toAddToTableIndex(1);
                    for (uint16 indCheck = 0; indCheck < 8; ++indCheck)
                    {
                        voxelCalc[indCheck] = getVoxel(posVoxel + toCheck[indCheck]); // <-- acc to valgrind getVoxel needs 85% of calculationTime;
                        if (voxelCalc[indCheck].getInterpolation() < isoLevel)
                        {
                            tableIndex |= toAddToTableIndex;
                        }
                        toAddToTableIndex*=2;
                    }

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
                                // we calucluate here everything in positive values; but normal correction starts @ -1
                                point *= voxelSize;
                                vertexGotCreated(posVoxel, voxelCalc, point);
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
                            if (vertex0 == vertex1 || vertex0 == vertex2 || vertex1 == vertex2)
                            {
                                continue; // triangle with zero space
                            }
                            const vector3 addNormal = (vertex1 - vertex0).crossProduct(vertex2 - vertex0);//.normalisedCopy();
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
            const int32 voxelLengthLodStart(t_voxelAccessor::voxelLengthLod-2);
            const int32 voxelLengthLodEnd(t_voxelAccessor::voxelLengthLod-1);
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
                const int32 vertexIndicesReuseLodSize(((t_voxelAccessor::voxelLength+1)*4)*
                                                      ((t_voxelAccessor::voxelLength+1)*4));
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
                                t_calcVoxelLod voxelCalc;
                                for (uint16 ind = 0; ind < 9; ++ind)
                                {
                                    const vector3int32 lookUp((voxelPos-start)+voxelLookups[coord][ind]);
                                    voxelCalc[ind] = getVoxelLod(lookUp, lod);
                                    if (voxelCalc[ind].getInterpolation() < isoLevel)
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

                                            //blub::BOUT("doing new transvoxel-vertex: resultPosition:" + blub::string::number(point));
                                            vertexGotCreatedLod(voxelPos, voxelCalc, point);
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

    /**
     * @brief clear erases all buffer/results.
     */
    virtual void clear()
    {
        m_positions.clear();
        m_normals.clear();
        m_indices.clear();
        for (int32 lod = 0; lod < 6; ++lod)
        {
            m_indicesLod[lod].clear();
        }
    }

    /**
     * @brief does the transvoxel algo get applied.
     * @return
     */
    bool getCaluculateTransvoxel() const
    {
        return m_lod > 0;
    }

    /**
     * @brief same as getCaluculateTransvoxel()
     * @see getCaluculateTransvoxel()
     */
    bool getCaluculateLod() const
    {
        return getCaluculateTransvoxel();
    }

    /**
     * @brief getPositions returns resulting position-list.
     * @return
     */
    const t_positionsList& getPositions() const
    {
        return m_positions;
    }
    /**
     * @brief getPositions returns resulting normal-list.
     * @return
     */
    const t_normalsList& getNormals() const
    {
        return m_normals;
    }
    /**
     * @brief getPositions returns resulting index-list.
     * @return
     */
    const t_indicesList& getIndices() const
    {
        return m_indices;
    }
    /**
     * @brief getPositions returns resulting transvoxel-list. Vertices for these indices are in getPositions() and getNormals().
     * @return
     */
    const t_indicesList& getIndicesLod(const uint16& lod) const
    {
        BASSERT(lod < 6);
        return m_indicesLod[lod];
    }

protected:
    /**
     * @brief surface constructor
     */
    surface()
    {
    }

    /**
     * @brief If you got a custom voxel, that effects the resulting surface derive this class and reimplement vertexGotCreated() and vertexGotCreatedLod().
     */
    virtual void vertexGotCreated(const vector3int32& /*voxelPos*/, const t_calcVoxel &/*voxel*/, vector3 &/*position*/)
    {
        ;
    }

    /**
     * @brief If you got a custom voxel, that effects the resulting surface derive this class and reimplement vertexGotCreated() and vertexGotCreatedLod().
     */
    virtual void vertexGotCreatedLod(const vector3int32& /*voxelPos*/, const t_calcVoxelLod &/*voxel*/, vector3 &/*position*/)
    {
        ;
    }

private:
    const voxelType &getVoxel(const vector3int32& pos) const
    {
        return m_voxel->getVoxel(pos);
    }
    const int8 &getVoxelInterpolation(const vector3int32& pos) const
    {
        return getVoxel(pos).getInterpolation();
    }
    const voxelType &getVoxelLod(const vector3int32& pos, const uint16 &lod) const
    {
        return m_voxel->getVoxelLod(pos, lod);
    }
    const int8 &getVoxelInterpolationLod(const vector3int32& pos, const uint16 &lod) const
    {
        return getVoxelLod(pos, lod).getInterpolation();
    }
    int32 calculateEdgeId(const vector3int32& pos, const int32& edgeInformation) const
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
    int32 calculateEdgeIdTransvoxel(const vector3int32& pos, const int32& edgeInformation, const int32& coord) const
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
    int32 calculateVertexId(const vector3int32& pos) const
    {
        return (
                ((pos.x+2)*3) * ((t_voxelAccessor::voxelLengthWithNormalCorrection)*3)*((t_voxelAccessor::voxelLengthWithNormalCorrection)*3) +
                ((pos.y+2)*3) * ((t_voxelAccessor::voxelLengthWithNormalCorrection)*3) +
                ((pos.z+2)*3)
                );
    }
    int32 calculateVertexIdTransvoxel(const vector2int32& pos) const
    {
        return (pos.x+1)*4*(t_voxelAccessor::voxelLength+1) +
               (pos.y+1)*4;
    }
    vector3 calculateIntersectionPosition(const vector3int32& pos, const int32& corner0, const int32& corner1)
    {
        const vector3int32 corn0(calculateCorner(corner0));
        const vector3int32 corn1(calculateCorner(corner1));

        const int8 interpolation0 = getVoxelInterpolation(corn0+pos);
        const int8 interpolation1 = getVoxelInterpolation(corn1+pos);

        const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

        return vector3(pos) + result;
    }
    vector3 calculateIntersectionPositionTransvoxel(const vector3int32& pos, const int32& corner0, const int32& corner1, const vector3int32 voxel[], const uint16 &lod)
    {
        const vector3int32 corn0(calculateCornerTransvoxel(corner0, voxel));
        const vector3int32 corn1(calculateCornerTransvoxel(corner1, voxel));

        const int8 interpolation0 = getVoxelInterpolationLod(corn0+pos, lod);
        const int8 interpolation1 = getVoxelInterpolationLod(corn1+pos, lod);

        const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

        return (vector3(pos) + result)/2.;
    }
    static vector3int32 calculateCorner(const int32& corner)
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
    static vector3int32 calculateCornerTransvoxel(const int32& corner, const vector3int32 voxel[])
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
    static vector3 getInterpolatedPosition(const vector3& positionFrom, const vector3& positionTo, const int8& interpolationFrom, const int8& interpolationTo)
    {
        BASSERT(interpolationTo != interpolationFrom);

        const real mu((0.0 - ((real)interpolationFrom)) / ((real)(interpolationTo - interpolationFrom)));
        const vector3 result = positionFrom + mu*(positionTo-positionFrom);

        return result;
    }

protected:
    t_voxelAccessorPtr m_voxel;
    int32 m_lod;

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
