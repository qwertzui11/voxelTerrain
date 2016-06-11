#include "blub/log/global.hpp"
#include "blub/log/system.hpp"
#include "blub/math/colour.hpp"
#include "blub/math/math.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/sphere.hpp"
#include "blub/math/transform.hpp"
#include "blub/serialization/callBaseObject.hpp"
#include "blub/sync/identifier.hpp"
#include "blub/procedural/voxel/config.hpp"
#include "blub/procedural/voxel/edit/axisAlignedBox.hpp"
#include "blub/procedural/voxel/edit/sphere.hpp"
#include "blub/procedural/voxel/simple/accessor.hpp"
#include "blub/procedural/voxel/simple/container/inMemory.hpp"
#include "blub/procedural/voxel/simple/renderer.hpp"
#include "blub/procedural/voxel/simple/surface.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/terrain/surface.hpp"
#include "blub/procedural/voxel/terrain/renderer.hpp"
#include "blub/procedural/voxel/tile/container.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"
#include "blub/procedural/voxel/tile/surface.hpp"

#include "OgreTile.hpp"
#include "Handler.hpp"

#include <boost/function.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>


/** @example customVertexInformation.cpp
 * This example shows how to add custom information to a voxel and pass them the vertices.
 * @image html voxel_customVoxelInformation.png
 */


using namespace blub::procedural;
using namespace blub;


template <typename configType>
class customOgreTile : public OgreTile<configType>
{
public:
    typedef configType t_config;

    typedef blub::sharedPointer<customOgreTile<t_config> > pointer;
    typedef OgreTile<t_config> t_base;

    static pointer create(Ogre::SceneManager *sc,
                          Ogre::String materialName,
                          blub::async::dispatcher *graphicDispatcher)
    {
        const pointer result(new customOgreTile(sc, materialName, *graphicDispatcher));
        result->initialise();
        return result;
    }

    void addCustomVertexDeclaration(Ogre::VertexDeclaration* decl)
    {
        decl->addElement(2, 0, Ogre::VET_COLOUR_ABGR, Ogre::VES_DIFFUSE);
    }
    void addCustomVertexInformation(Ogre::VertexBufferBinding* binding, const typename t_base::t_vertices& vertices)
    {
        t_base::addCustomVertexInformation(binding, vertices);
        {
            Ogre::HardwareVertexBufferSharedPtr diffuseBuffer;
            const size_t sizeVertex = Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR_ABGR);

            diffuseBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        sizeVertex, vertices.size(), Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            uint8* toWriteTo(static_cast<uint8*>(diffuseBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
            for (uint32 ind = 0; ind < vertices.size(); ++ind)
            {
                const uint32 indColour = ind*4;
                const typename t_base::t_vertex& vertex(vertices.at(ind));
                toWriteTo[indColour+0] = uint8(vertex.diffuse.r*255.);
                toWriteTo[indColour+1] = uint8(vertex.diffuse.g*255.);
                toWriteTo[indColour+2] = uint8(vertex.diffuse.b*255.);
                toWriteTo[indColour+3] = uint8(vertex.diffuse.a*255.);
            }
            diffuseBuffer->unlock();

            binding->setBinding(2, diffuseBuffer);
        }
    }
protected:
    customOgreTile(Ogre::SceneManager *sc,
                   Ogre::String materialName,
                   blub::async::dispatcher &graphicDispatcher)
        : t_base(sc, materialName, graphicDispatcher)
    {;}
};

template <typename configType>
class customSurfaceTile : public voxel::tile::surface<configType>
{
public:
    typedef configType t_config;
    typedef blub::sharedPointer<customSurfaceTile<t_config> > pointer;
    typedef voxel::tile::surface<t_config> t_base;

    static pointer create()
    {
        return pointer(new customSurfaceTile());
    }
    static pointer createCopy(pointer toCopy)
    {
        return pointer(new customSurfaceTile(*toCopy.get()));
    }
    typename t_base::t_vertex createVertex(const vector3int32& voxelPos, const typename t_base::t_voxel &voxel0, const typename t_base::t_voxel &voxel1, const vector3 &position, const vector3 &normal)
    {
        // the algorithm calculates the voxel position depending on two voxels (on their edge)
        typename t_base::t_vertex result(t_base::createVertex(voxelPos, voxel0, voxel1, position, normal));
        if (voxel0.diffuse == colour(1.,1.,1.,1.))
        {result.diffuse = voxel1.diffuse;}
        else
        {result.diffuse = voxel0.diffuse;}
        return result;
    }
    typename t_base::t_vertex createVertexLod(const vector3int32& voxelPos, const typename t_base::t_voxel &voxel0, const typename t_base::t_voxel &voxel1, const vector3 &position, const vector3 &normal)
    {
        typename t_base::t_vertex result(t_base::createVertexLod(voxelPos, voxel0, voxel1, position, normal));
        result.diffuse = (voxel0.diffuse + voxel1.diffuse) / 2;
        return result;
    }
protected:
    customSurfaceTile() = default;
};

template <typename configType>
class customEdit : public voxel::edit::sphere<configType>
{
public:
    typedef configType t_config;
    typedef blub::sharedPointer<customEdit<t_config> > pointer;
    typedef voxel::edit::sphere<configType> t_base;

    static pointer create(const ::blub::sphere& desc)
    {
        return pointer(new customEdit(desc));
    }
    bool calculateOneVoxel(const vector3& pos, typename t_base::t_voxel* resultVoxel) const override
    {
        const bool result(t_base::calculateOneVoxel(pos, resultVoxel));
        if (!result)
        {
            return false;
        }
        resultVoxel->diffuse = diffuseToSet;
        return true;
    }
protected:
    customEdit(const ::blub::sphere& desc)
        : t_base(desc)
    {
        const uint32 colourToRand = math::rand()%3;
        switch (colourToRand)
        {
        case 0:
            diffuseToSet.r = math::randReal(); break;
        case 1:
            diffuseToSet.g = math::randReal(); break;
        case 2:
            diffuseToSet.b = math::randReal(); break;
        default:
            BASSERT(true);
        }
//        diffuseToSet.r = math::randReal();
//        diffuseToSet.g = math::randReal();
//        diffuseToSet.b = math::randReal();
    }

    colour diffuseToSet;
};

struct data : public voxel::data
{
    data()
        : voxel::data()
        , diffuse(1., 1., 1.) // default colour white
    {;}
    colour diffuse; // A diffuse colour gets added to every voxel
};

struct vertex : public voxel::vertex
{
    vertex() = default;

    colour diffuse; // A diffuse colour gets added to every vertex
};

struct config : public voxel::config
{
    typedef data t_data; // use custom voxel
    typedef vertex t_vertex; // use custom vertex

    typedef container<config> t_container;
    typedef accessor<config> t_accessor;
    template <typename configType>
    struct surface : public voxel::config::surface<configType>
    {
        typedef customSurfaceTile<configType> t_tile;
    };
    typedef surface<config> t_surface;
    template <typename configType>
    struct renderer : public voxel::config::renderer<configType>
    {
        typedef customOgreTile<configType> t_tile;
    };
    typedef renderer<config> t_renderer;
};
typedef customEdit<config> t_customEdit;

typedef sharedPointer<sync::identifier> t_cameraIdentifier;
typedef config t_config;
typedef voxel::simple::container::inMemory<t_config> t_voxelContainer;
typedef voxel::terrain::accessor<t_config> t_voxelAccessor;
typedef voxel::terrain::renderer<t_config> t_voxelRenderer;
typedef voxel::terrain::surface<t_config> t_voxelSurface;
typedef voxel::edit::axisAlignedBox<t_config> t_editAxisAlignedBox;
typedef voxel::edit::sphere<t_config> t_editSphere;
typedef t_config::t_renderer::t_tile t_renderTile;


void createSphere(t_voxelContainer *container, const vector3 &position, const bool &cut);


int main(int /*argc*/, char* /*argv*/[])
{
    // starts logging to console and file. blub::log::system uses boost::log.
    blub::log::system::addConsole();
    blub::log::system::addFile("voxelterrain.log");

    // class Handler initialises Ogre3d and OIS and handles their callbacks.
    Handler handler;
    if (!handler.initialiseOgre())
    {
        return EXIT_FAILURE;
    }
    handler.camera->setPosition(vector3(20., 0., -50.));
    if (!handler.initialiseOIS())
    {
        return EXIT_FAILURE;
    }

    // initialise terrain
    scopedPointer<t_voxelContainer> voxelContainer; // contains the voxel
    scopedPointer<t_voxelAccessor> voxelAccessor; // is an optimised buffer for the surface calculation. Takes the voxel needed for the surface calculation from the voxelContainer and caches them.
    scopedPointer<t_voxelSurface> voxelSurface; // Takes the optimsed data from voxelAccessor and creates an iso-surface out of it.
    scopedPointer<t_voxelRenderer> voxelRenderer; // handles the rendering. Checks if a tile is too far away or too near. Handles when to render a crack-closing subentity.

    // does all the terrain work on 4 threads.
    async::dispatcher terrainDispatcher(4, false, "terrain");

    t_cameraIdentifier cameraIdentifier;
    {
        const int32 numLod(3);

        // voxel themself
        voxelContainer.reset(new t_voxelContainer(terrainDispatcher));

        // accessor
        voxelAccessor.reset(new t_voxelAccessor(terrainDispatcher, *voxelContainer, numLod));

        // surface
        voxelSurface.reset(new t_voxelSurface(terrainDispatcher, *voxelAccessor));

        // create material
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("customVertexInformation", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::Pass *pass = material->getTechnique(0)->getPass(0);
//        pass->setAmbient(1, 1, 0);
//        pass->setDiffuse(1, 1, 0, 1);
        pass->setVertexColourTracking(Ogre::TVC_AMBIENT);
        pass->setLightingEnabled(true);

        // ogre3d render wrapper
        const t_voxelRenderer::t_createTileCallback callbackCreate = boost::bind(t_renderTile::create, handler.renderScene, "customVertexInformation", &handler.graphicDispatcher);

        // renderer
        t_voxelRenderer::t_syncRadiusList lodRadien(numLod); // defines how far a tile on a lod has to be away to get rendered.
        lodRadien[0] = t_config::voxelsPerTile*2.0;
        lodRadien[1] = t_config::voxelsPerTile*2.0;
        lodRadien[2] = t_config::voxelsPerTile*2.0;
        voxelRenderer.reset(new t_voxelRenderer(terrainDispatcher, *voxelSurface, lodRadien));
        voxelRenderer->setCreateTileCallback(callbackCreate); // TODO must be set - shouldnt
        cameraIdentifier = sync::identifier::create();
        voxelRenderer->addCamera(cameraIdentifier, handler.camera->getPosition());
        handler.signalFrame()->connect(
                    [&] (real)
                    {
                        voxelRenderer->updateCamera(cameraIdentifier, handler.camera->getPosition());
                    }
        );

        // add/cut sphere when mouse-button gets pressed
        handler.signalMouseGotPressed()->connect(
                    [&] (bool left)
                    {
                        createSphere(voxelContainer.get(), handler.camera->getPosition()+handler.camera->getDirection()*10., !left);
                    }
        );
    }

    // create voxel
    {
        // spheres
        t_customEdit::pointer sphereEdit(t_customEdit::create(sphere(vector3(), 2.)));
        t_customEdit::pointer sphereEditCut(t_customEdit::create(sphere(vector3(), 2.)));
        sphereEditCut->setCut(true);

        voxelContainer->editVoxel(sphereEdit);
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(10., 0., 0.),   quaternion(), 2.));
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(20., 0., 0.),   quaternion(), 4.));
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(35., 0., 0.),   quaternion(), 8.));
        voxelContainer->editVoxel(sphereEditCut, transform(vector3(35., 0., -16.), quaternion(), 6.));

        // axis aligned boxes
        t_editAxisAlignedBox::pointer aabEdit(t_editAxisAlignedBox::create(blub::axisAlignedBox(vector3(-2.), vector3(2.))));

        voxelContainer->editVoxel(aabEdit, transform(vector3(0., 0., 15.)));
        voxelContainer->editVoxel(aabEdit, transform(vector3(10., 0., 15.), quaternion(), 2.));
        voxelContainer->editVoxel(aabEdit, transform(vector3(20., 0., 15.), quaternion(), 4.));
        voxelContainer->editVoxel(aabEdit, transform(vector3(35., 0., 15.), quaternion(), 8.));
    }


    // start rendering and begin terrain worker.
    {
        terrainDispatcher.start(); // NOW it starts creating the voxel.
        handler.renderSystem->startRendering();
        terrainDispatcher.stop();
    }

    return EXIT_SUCCESS;
}


void createSphere(t_voxelContainer *container, const vector3 &position, const bool &cut)
{
    t_customEdit::pointer sphereEdit(t_customEdit::create(sphere(position, 5.)));
    sphereEdit->setCut(cut);
    container->editVoxel(sphereEdit);
}



