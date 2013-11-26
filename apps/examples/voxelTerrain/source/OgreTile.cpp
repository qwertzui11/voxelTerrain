#include "OgreTile.hpp"

#include "blub/async/dispatcher.hpp"
#include "blub/math/axisAlignedBox.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreHardwareBuffer.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreHardwareIndexBuffer.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreSubMesh.h>


using namespace blub;


OgreTile::OgreTile(Ogre::SceneManager *sc,
                   Ogre::String materialName,
                   blub::async::dispatcher &graphicDispatcher,
                   const blub::int32 &lod,
                   const blub::vector3int32 &id)
    : t_base(lod, id)
    , m_graphicDispatcher(graphicDispatcher)
    , m_materialName(materialName)
    , m_scene(sc)
    , m_mesh(nullptr)
    , m_entity(nullptr)
    , m_node(nullptr)
{
    for (int32 ind = 0; ind < 6; ++ind)
    {
        m_indexLodSubMesh[ind] = -1;
    }
}


OgreTile::t_base::pointer OgreTile::create(Ogre::SceneManager *sc,
                                           Ogre::String materialName,
                                           async::dispatcher *graphicDispatcher,
                                           const blub::int32 &lod,
                                           const blub::vector3int32 &id)
{
    pointer result(new OgreTile(sc, materialName, *graphicDispatcher, lod, id));
    result.staticCast<OgreTile>()->initialise();
    return result;
}

OgreTile::~OgreTile()
{
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::destroyAllGraphic, m_scene, m_mesh, m_entity, m_node));
}

void OgreTile::initialise()
{
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::createMeshGraphic, getSharedThisPtr().staticCast<OgreTile>()));
}

void OgreTile::setTileData(OgreTile::t_tileData *convertToRenderAble, const axisAlignedBox &aabb)
{
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setTileDataGraphic, getSharedThisPtr().staticCast<OgreTile>(), convertToRenderAble, aabb));
}

void OgreTile::setVisible(const bool &vis)
{
    t_base::setVisible(vis);
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setVisibleGraphic, getSharedThisPtr().staticCast<OgreTile>(), vis));
}

void OgreTile::setVisibleLod(const uint16 &indLod, const bool &vis)
{
    if (m_lodShouldBeVisible[indLod] == vis)
    {
        return; // nothing todo
    }
    t_base::setVisibleLod(indLod, vis);
    m_graphicDispatcher.dispatch(boost::bind(&OgreTile::setVisibleLodGraphic, getSharedThisPtr().staticCast<OgreTile>(), indLod, vis));
}


void OgreTile::createMeshGraphic()
{
    m_mesh = Ogre::MeshManager::getSingleton().createManual(
                "voxel_tile_" + string::number(m_lod) + "_" + string::number(m_id.x) + "_" + string::number(m_id.y) + "_" + string::number(m_id.z),
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    m_node = m_scene->getRootSceneNode();
}

void OgreTile::setTileDataGraphic(OgreTile::t_tileData *convertToRenderAble, const axisAlignedBox &aabb)
{
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
        const t_tileData::t_positionsList positions(convertToRenderAble->getPositions());
        const t_tileData::t_normalsList normals(convertToRenderAble->getNormals());
        const t_tileData::t_indicesList indices(convertToRenderAble->getIndices());

        BASSERT(positions.size() >= 3);
        BASSERT(positions.size() >= 3);
        BASSERT(positions.size() == normals.size());
        BASSERT(indices.size() >= 3);
        BASSERT(indices.size() % 3 == 0);

        Ogre::MeshPtr meshWork(m_mesh);

        meshWork->_setBounds(aabb, false);
        meshWork->_setBoundingSphereRadius(aabb.getSize().length()/2.);

        if (meshWork->sharedVertexData == nullptr)
        {
            meshWork->sharedVertexData = new Ogre::VertexData();

            {
                Ogre::VertexDeclaration* decl = meshWork->sharedVertexData->vertexDeclaration;

                decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
                decl->addElement(1, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
            }
        }

        meshWork->sharedVertexData->vertexCount = positions.size();
        Ogre::VertexBufferBinding* bind = meshWork->sharedVertexData->vertexBufferBinding;
        {
            Ogre::HardwareVertexBufferSharedPtr positionBuffer;
            const size_t sizeVertex = Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

            positionBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        sizeVertex, meshWork->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            vector3* toWriteTo(static_cast<vector3*>(positionBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            memcpy(toWriteTo, positions.data(), sizeof(vector3) * meshWork->sharedVertexData->vertexCount);
            positionBuffer->unlock();

            bind->setBinding(0, positionBuffer);
        }
        {
            Ogre::HardwareVertexBufferSharedPtr normalBuffer;
            const size_t sizeVertex = Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

            normalBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        sizeVertex, meshWork->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            vector3* toWriteTo(static_cast<vector3*>(normalBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            memcpy(toWriteTo, normals.data(), sizeof(vector3) * meshWork->sharedVertexData->vertexCount);
            normalBuffer->unlock();

            bind->setBinding(1, normalBuffer);
        }

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
                const t_tileData::t_indicesList& indicesLod(convertToRenderAble->getIndicesLod(indSubMesh-1));
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
                    const t_tileData::t_indicesList& indicesLod(convertToRenderAble->getIndicesLod(indSubMesh-1));
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
    m_entity->setVisible(wasVisible);

    m_node->attachObject(m_entity);
    for (int32 indLod = 0; indLod < 6; ++indLod)
    {
        if (m_indexLodSubMesh[indLod] != -1)
        {
            m_entity->getSubEntity(m_indexLodSubMesh[indLod])->setVisible(m_lodShouldBeVisible[indLod]);
        }
    }
}

void OgreTile::setVisibleGraphic(const bool& vis)
{
    m_entity->setVisible(vis);
}

void OgreTile::setVisibleLodGraphic(const uint16& indLod, const bool& vis)
{
    if (m_indexLodSubMesh[indLod] != -1)
    {
        m_entity->getSubEntity(m_indexLodSubMesh[indLod])->setVisible(vis);
    }
}

void OgreTile::destroyAllGraphic(Ogre::SceneManager* scene, Ogre::MeshPtr mesh_, Ogre::Entity *entity_, Ogre::SceneNode *node_)
{
    scene->destroyEntity(entity_);
    Ogre::MeshManager::getSingleton().remove(mesh_->getName());

    (void)node_;
}
