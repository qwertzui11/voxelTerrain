#ifndef OGRETILE_HPP
#define OGRETILE_HPP

#include "blub/async/dispatcher.hpp"
#include "blub/async/predecl.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/log/global.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/vector3int.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"
#include "blub/procedural/voxel/data.hpp"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreHardwareBuffer.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreHardwareIndexBuffer.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreSubMesh.h>


blub::uint32 g_meshId = 0;


/**
 * @brief The OgreTile class converts the resulting vertices and indices of the voxel-terrain to the Ogre Hardwarebuffer.
 * The class handles setVisible() when a tile gets cutted because it's too near or too far or a cracks has to get closed.
 * The results of the transvoxel-algorithm for closing the cracks between the lod-tiles get set to submeshes.
 * Every tile contains a Ogre::Mesh, a Ogre::Entity and a Ogre::SceneNode.
 * For more information on how to use ogre3d see http://www.ogre3d.org/docs/manual/ and http://www.ogre3d.org/docs/api/1.9/ .
 */
template <typename configType = blub::procedural::voxel::config>
class OgreTile : public blub::procedural::voxel::tile::renderer<configType>
{
public:   
    typedef configType t_config;
    typedef blub::sharedPointer<OgreTile<t_config> > pointer;
    typedef blub::sharedPointer<const OgreTile<t_config> > constPointer;
    typedef blub::procedural::voxel::tile::renderer<t_config> t_base;
    typedef typename t_config::t_renderer::t_tile* t_thiz;
    typedef typename t_config::t_surface::t_tile t_voxelSurfaceTile;
    typedef typename t_base::t_tileData::t_vertices t_vertices;
    typedef typename t_config::t_vertex t_vertex;

    /**
     * @brief create creates an instance and calls initialise().
     * @param sc The ogre scene. Must not be nullptr;
     * @param materialName The ogre material-name.
     * @param graphicDispatcher Only the thread that initialised the Ogre::Root may call Ogre methods.
     * The graphicDispatcher must get called on every frame by the ogre thread.
     * Methods that end with Graphic have to get called by this dispatcher.
     * @return returns an instance. Never nullptr.
     */
    static pointer create(Ogre::SceneManager *sc,
                          Ogre::String materialName,
                          blub::async::dispatcher *graphicDispatcher); // needed because only Ogre3d thread may call ogre3d methods
    /**
     * @brief ~OgreTile desctructor. Calls the static method destroyAllGraphic() to ensure Ogre instances get deleted by the right thread.
     * The graphicDispatcher set by create() must stay valid until after ogre3d shutdown.
     */
    virtual ~OgreTile();

    void setTileData(typename t_base::t_tileDataPtr convertToRenderAble, const blub::axisAlignedBox &aabb);

    void setVisible(const bool& vis) override;
    void setVisibleLod(const blub::uint16& indLod, const bool& vis) override;

protected:
    /**
     * @brief OgreTile constrcutor
     * @see create().
     */
    OgreTile(Ogre::SceneManager *sc,
             Ogre::String materialName,
             blub::async::dispatcher &graphicDispatcher);
    /**
     * @brief initialise calls createMeshGraphic() on graphic thread.
     * Not called by constructor because getSharedThisPtr() isn't yet available.
     */
    void initialise();

    /**
     * @brief createMeshGraphic allocates and initialises an Ogre::Mesh where the vertices and indices will get saved to later.
     */
    void createMeshGraphic();
    /**
     * @brief setTileDataGraphic convertes a vector of vertices and indices to ogres hardware buffer.
     * @param convertToRenderAble To convert.
     * @param aabb Used for the meshes aabb and the center gets used as the position for the Ogre::SceneNode.
     */
    void setTileDataGraphic(blub::sharedPointer<t_voxelSurfaceTile> convertToRenderAble, const blub::axisAlignedBox &aabb);
    /**
     * @brief setVisibleGraphic sets the whole tile to visible or invisible. Gets called when tile cutted because too near or too far away.
     * @param vis
     */
    void setVisibleGraphic(const bool& vis);
    /**
     * @brief setVisibleLodGraphic sets a subentity-visibility. Gets called if the neighbour tile has a different LOD.
     * To close the occuring cracks this method gets called.
     * @param indLod
     * @param vis
     */
    void setVisibleLodGraphic(const blub::uint16& indLod, const bool& vis);
    /**
     * @brief destroyAllGraphic gets called by destructor - ensures that ogre3d classes get destroyed by the right thread.
     * @param scene Must not be nullptr.
     * @param mesh_ Must not be nullptr.
     * @param entity_ Must not be nullptr.
     * @param node_ Must not be nullptr.
     */
    static void destroyAllGraphic(Ogre::SceneManager* scene, Ogre::MeshPtr mesh_, Ogre::Entity *entity_, Ogre::SceneNode *node_);

    void addCustomVertexInformation(Ogre::VertexBufferBinding* binding, const t_vertices& vertices) {;}
    void addCustomVertexDeclaration(Ogre::VertexDeclaration* decl) {;}

    constPointer getSharedThisPtr() const;
    pointer getSharedThisPtr();

private:
    blub::async::dispatcher &m_graphicDispatcher;

    Ogre::String m_materialName;
    Ogre::SceneManager* m_scene;
    Ogre::MeshPtr m_mesh;
    Ogre::Entity* m_entity;
    Ogre::SceneNode* m_node;

    blub::int32 m_indexLodSubMesh[6];
};


template <typename configType>
OgreTile<configType>::OgreTile(Ogre::SceneManager *sc,
                   Ogre::String materialName,
                   blub::async::dispatcher &graphicDispatcher)
    : m_graphicDispatcher(graphicDispatcher)
    , m_materialName(materialName)
    , m_scene(sc)
    , m_entity(nullptr)
    , m_node(nullptr)
{
    for (blub::int32 ind = 0; ind < 6; ++ind)
    {
        m_indexLodSubMesh[ind] = -1;
    }
}


template <typename configType>
typename OgreTile<configType>::pointer OgreTile<configType>::create(Ogre::SceneManager *sc,
                                                                  Ogre::String materialName,
                                                                  blub::async::dispatcher *graphicDispatcher)
{
    BASSERT(graphicDispatcher != nullptr);
    pointer result(new OgreTile(sc, materialName, *graphicDispatcher));
    result->initialise();
    return result;
}

template <typename configType>
OgreTile<configType>::~OgreTile()
{
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::destroyAllGraphic, m_scene, m_mesh, m_entity, m_node));
}

template <typename configType>
void OgreTile<configType>::initialise()
{
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::createMeshGraphic, getSharedThisPtr()));
}

template <typename configType>
void OgreTile<configType>::setTileData(typename t_base::t_tileDataPtr convertToRenderAble, const blub::axisAlignedBox &aabb)
{
    typename t_voxelSurfaceTile::pointer convertToRenderAbleCasted(convertToRenderAble.template staticCast<t_voxelSurfaceTile>());
    typename t_voxelSurfaceTile::pointer copy(t_voxelSurfaceTile::createCopy(convertToRenderAbleCasted));
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setTileDataGraphic, getSharedThisPtr(), copy, aabb));
}

template <typename configType>
void OgreTile<configType>::setVisible(const bool &vis)
{
    t_base::setVisible(vis);
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setVisibleGraphic, getSharedThisPtr(), vis));
}

template <typename configType>
void OgreTile<configType>::setVisibleLod(const blub::uint16 &indLod, const bool &vis)
{
    if (t_base::m_lodShouldBeVisible[indLod] == vis)
    {
        return; // nothing todo
    }
    t_base::setVisibleLod(indLod, vis);
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setVisibleLodGraphic, getSharedThisPtr(), indLod, vis));
}


template <typename configType>
void OgreTile<configType>::createMeshGraphic()
{
    m_mesh = Ogre::MeshManager::getSingleton().createManual(
                blub::string("voxel_") + blub::string::number(g_meshId++),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    m_node = m_scene->getRootSceneNode()->createChildSceneNode();
}

template <typename configType>
void OgreTile<configType>::setTileDataGraphic(blub::sharedPointer<t_voxelSurfaceTile> convertToRenderAble, const blub::axisAlignedBox &aabb)
{
    using namespace blub;

    bool wasVisible(false);
    if (m_entity)
    {
        wasVisible = m_entity->getVisible();

        delete m_entity;
        m_entity = nullptr;
    }

    uint32 numSubmeshes(1);
    if (convertToRenderAble->getCaluculateLod())
    {
        for (int32 indLod = 0; indLod < 6; ++indLod)
        {
            if (convertToRenderAble->getIndicesLod(indLod).size() > 0)
            {
                ++numSubmeshes;
            }
        }
    }

    {
        const t_vertices vertices(convertToRenderAble->getVertices());
        const typename t_base::t_tileData::t_indices indices(convertToRenderAble->getIndices());

        BASSERT(vertices.size() >= 3);
        BASSERT(indices.size() >= 3);
        BASSERT(indices.size() % 3 == 0);

        Ogre::MeshPtr meshWork(m_mesh);

        const vector3 aabbHalfSize(aabb.getHalfSize());
        meshWork->_setBounds(axisAlignedBox(-aabbHalfSize, aabbHalfSize), false);
        // meshWork->_setBounds(aabb, false);
        const real radius(aabbHalfSize.length());
        meshWork->_setBoundingSphereRadius(radius);
        // BLUB_LOG_OUT() << "aabbHalfSize:" << aabbHalfSize;

        if (meshWork->sharedVertexData == nullptr)
        {
            meshWork->sharedVertexData = new Ogre::VertexData();

            {
                Ogre::VertexDeclaration* decl = meshWork->sharedVertexData->vertexDeclaration;

                decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                decl->addElement(1, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
                static_cast<t_thiz>(this)->addCustomVertexDeclaration(decl);
            }
        }

        meshWork->sharedVertexData->vertexCount = vertices.size();
        Ogre::VertexBufferBinding* bind = meshWork->sharedVertexData->vertexBufferBinding;
        {
            Ogre::HardwareVertexBufferSharedPtr positionBuffer;
            const size_t sizeVertex = Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

            positionBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        sizeVertex, meshWork->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            vector3* toWriteTo(static_cast<vector3*>(positionBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            for (uint32 ind = 0; ind < vertices.size(); ++ind)
            {
                toWriteTo[ind] = vertices.at(ind).position - aabbHalfSize;
            }
            positionBuffer->unlock();

            bind->setBinding(0, positionBuffer);
        }
        {
            Ogre::HardwareVertexBufferSharedPtr normalBuffer;
            const size_t sizeVertex = Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

            normalBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        sizeVertex, meshWork->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            vector3* toWriteTo(static_cast<vector3*>(normalBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            for (uint32 ind = 0; ind < vertices.size(); ++ind)
            {
                toWriteTo[ind] = vertices.at(ind).normal;
            }
            normalBuffer->unlock();

            bind->setBinding(1, normalBuffer);
        }
        static_cast<t_thiz>(this)->addCustomVertexInformation(bind, vertices);

        if (numSubmeshes < meshWork->getNumSubMeshes())
        {
            for (int32 ind = 0; meshWork->getNumSubMeshes() - numSubmeshes; ++ind)
            {
                meshWork->destroySubMesh(0);
            }
        }
        if (numSubmeshes > meshWork->getNumSubMeshes())
        {
            for (int32 ind = 0; numSubmeshes - meshWork->getNumSubMeshes(); ++ind)
            {
                Ogre::SubMesh* sub(meshWork->createSubMesh());
                sub->setBuildEdgesEnabled(false);
                sub->useSharedVertices = true;
            }
        }
        BASSERT(numSubmeshes == meshWork->getNumSubMeshes());

        for (int32 indLod = 0; indLod < 6; ++indLod)
        {
            m_indexLodSubMesh[indLod] = -1;
        }

        int32 indexSubMesh(0);
        uint32 numIterations(7);
        if (numSubmeshes == 1)
        {
            numIterations = 1;
        }
        for (uint32 indSubMesh = 0; indSubMesh < numIterations; ++indSubMesh)
        {
            uint16 numIndices;
            if (indSubMesh == 0)
            {
                BASSERT(indices.size() < 65536);
                numIndices = indices.size();
            }
            else
            {
                const typename t_base::t_tileData::t_indices& indicesLod(convertToRenderAble->getIndicesLod(indSubMesh-1));
                BASSERT(indicesLod.size() < 65536);
                if (indicesLod.empty())
                {
                    continue;
                }
                BASSERT(indicesLod.size() % 3 == 0);
                numIndices = indicesLod.size();
            }

            Ogre::HardwareIndexBufferSharedPtr indexBuffer = Ogre::HardwareBufferManager::getSingleton().
                    createIndexBuffer(
                        Ogre::HardwareIndexBuffer::IT_16BIT,
                        numIndices,
                        Ogre::HardwareBuffer::HBU_STATIC);
            uint16* toWriteTo(static_cast<uint16*>(indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            {
                if (indSubMesh == 0)
                {
                    memcpy(toWriteTo, indices.data(), sizeof(uint16) * numIndices);
                }
                else
                {
                    const typename t_base::t_tileData::t_indices& indicesLod(convertToRenderAble->getIndicesLod(indSubMesh-1));
                    memcpy(toWriteTo, indicesLod.data(), sizeof(uint16) * numIndices);
                }
            }
            indexBuffer->unlock();

            Ogre::SubMesh* sub = meshWork->getSubMesh(indexSubMesh);
            sub->indexData->indexBuffer = indexBuffer;
            sub->indexData->indexCount = numIndices;
            sub->indexData->indexStart = 0;
            sub->setMaterialName(m_materialName);

            if (indSubMesh > 0)
            {
                m_indexLodSubMesh[indSubMesh-1] = indexSubMesh;
            }
            ++indexSubMesh;
        }
    }

    m_entity = m_scene->createEntity(m_mesh);
    m_node->setPosition(aabb.getCenter());
    m_node->attachObject(m_entity);
    setVisibleGraphic(wasVisible);

    for (int32 indLod = 0; indLod < 6; ++indLod)
    {
        if (m_indexLodSubMesh[indLod] != -1)
        {
            m_entity->getSubEntity(m_indexLodSubMesh[indLod])->setVisible(t_base::m_lodShouldBeVisible[indLod]);
        }
    }
}

template <typename configType>
void OgreTile<configType>::setVisibleGraphic(const bool& vis)
{
    m_node->setVisible(vis);
}

template <typename configType>
void OgreTile<configType>::setVisibleLodGraphic(const blub::uint16& indLod, const bool& vis)
{
    if (m_indexLodSubMesh[indLod] != -1)
    {
        m_entity->getSubEntity(m_indexLodSubMesh[indLod])->setVisible(vis);
    }
}

template <typename configType>
void OgreTile<configType>::destroyAllGraphic(Ogre::SceneManager* scene, Ogre::MeshPtr mesh_, Ogre::Entity *entity_, Ogre::SceneNode *node_)
{
    scene->destroyEntity(entity_);
    Ogre::MeshManager::getSingleton().remove(mesh_->getName());

    scene->destroySceneNode(node_);
}

template <typename configType>
blub::sharedPointer<const OgreTile<configType> > OgreTile<configType>::getSharedThisPtr() const
{
    return t_base::getSharedThisPtr().template staticCast<const OgreTile<configType> >();
}
template <typename configType>
blub::sharedPointer<OgreTile<configType> > OgreTile<configType>::getSharedThisPtr()
{
    return t_base::getSharedThisPtr().template staticCast<OgreTile<configType> >();
}



#endif // OGRETILE_HPP
