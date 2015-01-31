#ifndef BLUB_PROCEDURAL_VOXEL_EDIT_MESH_HPP
#define BLUB_PROCEDURAL_VOXEL_EDIT_MESH_HPP

#include "blub/log/global.hpp"
#include "blub/core/map.hpp"
#include "blub/core/pair.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/vector.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/intersection.hpp"
#include "blub/math/plane.hpp"
#include "blub/math/ray.hpp"
#include "blub/math/transform.hpp"
#include "blub/math/triangleVector3.hpp"
#include "blub/procedural/voxel/edit/base.hpp"
#include "blub/procedural/voxel/tile/container.hpp"

#include <utility>

#ifdef BLUB_USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#endif

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/index/rtree.hpp>


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace edit
{


/**
 * @brief convertes a mesh to voxel. Fills a boost::geometry::index::rtree with the polygons.
 * Initialization O(numTriangles). Voxel-generation O(numVoxel)
 */
template <class voxelType>
class mesh : public base<voxelType>
{
public:
    typedef sharedPointer<mesh<voxelType> > pointer;

    typedef base<voxelType> t_base;

    typedef tile::container<voxelType> t_tileContainer;

    typedef boost::geometry::model::point<real, 3, boost::geometry::cs::cartesian> t_point;
    typedef boost::geometry::model::box<t_point> t_box;
    typedef boost::geometry::model::segment<t_point> t_segment;
    typedef std::pair<t_box, triangleVector3> t_value;
    typedef boost::geometry::index::rtree<t_value, boost::geometry::index::quadratic<16> > t_tree;

    /**
     * @brief creates an instance of the class.
     * @return a shared_ptr of an instance
     */
    static pointer create()
    {
        return pointer(new mesh());
    }
    /**
     * @brief ~mesh destructor
     */
    virtual ~mesh()
    {
        for (t_trees::const_iterator it = m_trees.cbegin(); it != m_trees.cend(); ++it)
        {
            delete (*it);
        }
    }

#ifdef BLUB_USE_ASSIMP
    /**
     * @brief loads an assimp scence from path
     * @param path has to be a valid path. Relative or absolute.
     * @return true if successfully read and loaded
     */
    bool load(const blub::string &path, const blub::transform &trans)
    {
        Assimp::Importer* importer(new Assimp::Importer());

        const aiScene *scene = importer->ReadFile(path.c_str(), aiProcess_Triangulate);
        if (scene == nullptr)
        {
            BLUB_PROCEDURAL_LOG_ERROR() << "scene == nullptr loader->GetErrorString():" << importer->GetErrorString();
            delete importer;
            return false;
        }

        bool result(loadScene(*scene, trans));

        delete importer;

        return result;
    }

    /**
     * @brief loads an assimp scene
     * @param scene Scene to load
     * @return true if successfully loaded
     */
    virtual bool loadScene(const aiScene &scene, const blub::transform &trans)
    {
        if (!scene.HasMeshes())
        {
            BLUB_PROCEDURAL_LOG_ERROR() << "!scene.HasMeshes()";
            return false;
        }

#ifdef BLUB_LOG_PROCEDURAL_VOXEL_EDIT_MESH
        BLUB_LOG_OUT() << "scene.mNumMeshes:" << scene.mNumMeshes << " scene.mRootNode->mNumChildren:" << scene.mRootNode->mNumChildren;
#endif

        for(uint32 indMesh = 0; indMesh < scene.mNumMeshes; ++indMesh)
        {
            aiMesh *mesh(scene.mMeshes[indMesh]);
            loadMesh(*mesh, trans); // ignore result
        }
        return true;
    }

    /**
     * @brief loads an assimp mesh
     * @param mesh Mesh to load
     * @return true if successfully loaded
     */
    virtual bool loadMesh(const aiMesh &mesh, const blub::transform &trans)
    {
        typedef vector<vector3> t_positions;

#ifdef BLUB_LOG_PROCEDURAL_VOXEL_EDIT_MESH
        BLUB_PROCEDURAL_LOG_OUT() << "mesh.mName:" << blub::string(mesh.mName.C_Str(), mesh.mName.length);
#endif
        if (!mesh.HasFaces())
        {
            BLUB_PROCEDURAL_LOG_ERROR() << "!scene.mMeshes[indMesh]->HasFaces()";
            return false;
        }
        if (!mesh.HasPositions())
        {
            BLUB_PROCEDURAL_LOG_ERROR() << "!mesh.HasPositions()";
            return false;
        }

        t_tree *tree = new t_tree();
        m_trees.push_back(tree);

        t_positions positions;
        positions.resize(mesh.mNumVertices);
        for (blub::uint32 indVertex = 0; indVertex < mesh.mNumVertices; ++indVertex)
        {
            const aiVector3D &posToCast(mesh.mVertices[indVertex]);
            vector3 casted(posToCast.x, posToCast.y, posToCast.z);
            casted *= trans.scale;
            positions[indVertex] = casted;
            m_aabb.merge(casted);
        }
        for (blub::uint32 indFace = 0; indFace < mesh.mNumFaces; ++indFace)
        {
            const aiFace &faceToCast(mesh.mFaces[indFace]);
            BASSERT(faceToCast.mNumIndices == 3);
            if (faceToCast.mNumIndices != 3)
            {
				BLUB_PROCEDURAL_LOG_WARNING() << "faceToCast.mNumIndices != 3";
                continue;
            }
            const triangleVector3 resultTriangle(   positions[faceToCast.mIndices[0]],
                                                    positions[faceToCast.mIndices[1]],
                                                    positions[faceToCast.mIndices[2]]);

            const blub::axisAlignedBox &aabb(resultTriangle.getAxisAlignedBoundingBox());
            const blub::vector3 &aabbMin(aabb.getMinimum());
            const blub::vector3 &aabbMax(aabb.getMaximum());
            t_box toInsert(t_point(aabbMin.x, aabbMin.y, aabbMin.z), t_point(aabbMax.x, aabbMax.y, aabbMax.z));
            tree->insert(std::make_pair(toInsert, resultTriangle));
        }

        return !tree->empty();
    }
#endif

protected:
    /**
     * @brief mesh contructor
     */
    mesh()
    {

    }

    /**
     * @brief getAxisAlignedBoundingBox returns the transformed bounding box that includes the mesh.
     * @param trans Transform of edit
     * @return
     */
    blub::axisAlignedBox getAxisAlignedBoundingBox(const transform &trans) const override
    {
        BLUB_LOG_OUT() << "trans:" << trans;

        return blub::axisAlignedBox(m_aabb.getMinimum()*trans.scale + trans.position,
                                    m_aabb.getMaximum()*trans.scale + trans.position);
    }

    /**
     * @brief calculateVoxel The tree, which got generated in load() gets intersected for every voxel line.
     * @param voxelContainer Container where to set the voxel in.
     * @param voxelContainerOffset Absolut position of the voxelContainer.
     * @param trans Transform
     */
    void calculateVoxel(t_tileContainer *voxelContainer, const vector3int32 &voxelContainerOffset, const transform &/*trans*/) const
    {
        const vector3int32 posContainerAbsolut(voxelContainerOffset*t_tileContainer::voxelLength);

        vector3int32 toLoop[] = {{1, t_tileContainer::voxelLength, t_tileContainer::voxelLength},
                                 {t_tileContainer::voxelLength, 1, t_tileContainer::voxelLength},
                                 {t_tileContainer::voxelLength, t_tileContainer::voxelLength, 1}
                                };
        vector3 rayDir[] = {{1., 0., 0.},
                            {0., 1., 0.},
                            {0., 0., 1.}};
        for (uint32 indMesh = 0; indMesh < m_trees.size(); ++indMesh)
        {
            t_tree *tree(static_cast<t_tree*>(m_trees[indMesh]));
            for (int32 indAxis = 0; indAxis < 3; ++indAxis)
            {
                for (int32 indX = 0; indX < toLoop[indAxis].x; ++indX)
                {
                    for (int32 indY = 0; indY < toLoop[indAxis].y; ++indY)
                    {
                        for (int32 indZ = 0; indZ < toLoop[indAxis].z; ++indZ)
                        {
                            const vector3int32 posVoxel(indX, indY, indZ);
                            vector3 posAbsolut(posContainerAbsolut + posVoxel);

                            // posAbsolut = posAbsolut / trans.scale;
                            // posAbsolut -= trans.position;

                            const ray test(posAbsolut, rayDir[indAxis]);

                            const vector3 segmentStart  (posAbsolut+rayDir[indAxis]*(-10000.)); // TODO: fix me after ray support for boost::geometry is of their todolist
                            const vector3 segmentEnd    (posAbsolut+rayDir[indAxis]*(+10000.));

                            t_segment segment(t_point(segmentStart.x, segmentStart.y, segmentStart.z), t_point(segmentEnd.x, segmentEnd.y, segmentEnd.z));
                            typedef std::vector<t_value> t_resultTriangles;
                            t_resultTriangles resultTriangles;
                            tree->query(boost::geometry::index::intersects(segment), std::back_inserter(resultTriangles));

                            typedef pair<real, blub::plane> t_cutPoint;
                            typedef vector<t_cutPoint> t_cutPoints;
                            t_cutPoints cutPoints;
                            vector3 cutPoint;
                            for (t_resultTriangles::const_iterator it = resultTriangles.cbegin(); it != resultTriangles.cend(); ++it)
                            {
                                const triangleVector3 &triangleWork(it->second);
                                if (intersection::intersect(test, triangleWork, &cutPoint))
                                {
                                    bool insert(true);
//                                    if (!cutPoints.empty())
//                                    {
//                                        const plane planeTriBefore(cutPoints.at(cutPoints.size()-1).second);
//                                        if (/*math::abs*/(planeTriBefore.normal.dotProduct(triangleWork.getPlane().normal)) < 0.)
//                                        {
//                                            insert = false;
//                                        }
//                                    }
                                    if (insert)
                                    {
                                        real cutPointStart = cutPoint[indAxis] - static_cast<real>(posContainerAbsolut[indAxis]);
                                        cutPoints.push_back(t_cutPoint(cutPointStart, triangleWork.getPlane()));
                                    }
                                }
                            }
                            if(cutPoints.size() < 2)
                            {
                                continue;
                            }

                            typedef vector<t_cutPoint> t_cutPointsFiltered;
                            t_cutPointsFiltered cutPointsFiltered;
                            for (t_cutPoints::const_iterator it = cutPoints.cbegin(); it != cutPoints.cend(); ++it)
                            {
                                if (cutPointsFiltered.empty())
                                {
                                    cutPointsFiltered.push_back(*it);
                                    continue;
                                }
                                if (math::abs(cutPointsFiltered.back().first - it->first) < 1e-4f)
                                {
                                    continue;
                                }
                                cutPointsFiltered.push_back(*it);
                            }

                            if(cutPointsFiltered.size()%2 != 0 && cutPointsFiltered.size() > 0)
                            {
								BLUB_PROCEDURAL_LOG_WARNING() << "cutPointsFiltered.size()%2 != 0 cutPointsFiltered.size():" << cutPointsFiltered.size();
                                cutPointsFiltered.pop_back();
                            }
                            for (t_cutPointsFiltered::const_iterator it = cutPointsFiltered.cbegin(); it != cutPointsFiltered.cend(); ++it)
                            {
                                real cutPointsSorted[2];
                                blub::plane cutPlanes[2];
                                cutPointsSorted[0] = it->first;
                                cutPlanes[0] = it->second;
                                ++it;
                                BASSERT(it != cutPointsFiltered.cend());
                                cutPointsSorted[1] = it->first;
                                cutPlanes[1] = it->second;

                                const real length(cutPointsSorted[1]-cutPointsSorted[0]);
                                if (length < 1.)
                                {
#ifdef BLUB_LOG_PROCEDURAL_VOXEL_EDIT_MESH
//                                    blub::BOUT("length < 1.");
#endif
//                                    continue;
                                }
                                //BLUB_LOG_OUT() << "creating line length:" << length;
                                switch (indAxis)
                                {
                                case 0:
                                    t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::x, cutPlanes[0], cutPlanes[1]);
                                    break;
                                case 1:
                                    t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::y, cutPlanes[0], cutPlanes[1]);
                                    break;
                                case 2:
                                    t_base::createLine(voxelContainer, posVoxel, cutPointsSorted[0], length, t_base::axis::z, cutPlanes[0], cutPlanes[1]);
                                    break;
                                default:
                                    BASSERT(false);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

protected:
    typedef vector<t_tree*> t_trees;
    t_trees m_trees;

    blub::axisAlignedBox m_aabb;

};


}
}
}
}


#endif // BLUB_PROCEDURAL_VOXEL_EDIT_MESH_HPP
