#include "blub/core/log.hpp"
#include "blub/log/system.hpp"
#include "blub/math/math.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/sphere.hpp"
#include "blub/math/transform.hpp"
#include "blub/sync/identifier.hpp"
#include "blub/procedural/voxel/edit/axisAlignedBox.hpp"
#include "blub/procedural/voxel/edit/box.hpp"
#include "blub/procedural/voxel/edit/sphere.hpp"
#include "blub/procedural/voxel/simple/container/inMemory.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/terrain/surface.hpp"
#include "blub/procedural/voxel/terrain/renderer.hpp"
#include "blub/procedural/voxel/tile/container.hpp"

#include "OgreTile.hpp"
#include "Handler.hpp"


/** @example primitives.cpp
 * This is an example that converts spheres and axis-aligined-boxes to voxels.
 * It shows how to initialise, create/remove voxel and how to render properly.
 * @image html voxel_primitive.png
 * @image html voxel_primitive_wire.png
 */





using namespace blub::procedural;
using namespace blub;


typedef sharedPointer<sync::identifier> t_cameraIdentifier;
typedef voxel::data t_voxel;
typedef voxel::simple::container::inMemory<t_voxel> t_voxelContainer;
typedef voxel::terrain::accessor<t_voxel> t_voxelAccessor;
typedef voxel::terrain::renderer<t_voxel> t_voxelRenderer;
typedef voxel::terrain::surface<t_voxel> t_voxelSurface;
typedef voxel::edit::base<t_voxel> t_voxelEdit;
typedef OgreTile<t_voxel> t_renderTile;


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

        // ogre3d render wrapper
        const t_voxelRenderer::t_createTileCallback callbackCreate = boost::bind(t_renderTile::create, handler.renderScene, "none", &handler.graphicDispatcher);

        // renderer
        t_voxelRenderer::t_syncRadiusList lodRadien(numLod); // defines how far a tile on a lod has to be away to be rendered.
        lodRadien[0] = voxel::tile::container<t_voxel>::voxelLength*2.0;
        lodRadien[1] = voxel::tile::container<t_voxel>::voxelLength*2.0;
        lodRadien[2] = voxel::tile::container<t_voxel>::voxelLength*2.0;
        voxelRenderer.reset(new t_voxelRenderer(terrainDispatcher, *voxelSurface, lodRadien));
        voxelRenderer->setCreateTileCallback(callbackCreate);
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
                    [&] (bool &left)
                    {
                        createSphere(voxelContainer.get(), handler.camera->getPosition()+handler.camera->getDirection()*10., !left);
                    }
        );
    }

    // create voxel
    {
        // spheres
        voxel::edit::sphere<t_voxel>::pointer sphereEdit(voxel::edit::sphere<t_voxel>::create(sphere(vector3(), 2.)));
        voxel::edit::sphere<t_voxel>::pointer sphereEditCut(voxel::edit::sphere<t_voxel>::create(sphere(vector3(), 2.)));
        sphereEditCut->setCut(true);

        voxelContainer->editVoxel(sphereEdit);
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(10., 0., 0.),   quaternion(), 2.));
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(20., 0., 0.),   quaternion(), 4.));
        voxelContainer->editVoxel(sphereEdit,    transform(vector3(35., 0., 0.),   quaternion(), 8.));
        voxelContainer->editVoxel(sphereEditCut, transform(vector3(35., 0., -16.), quaternion(), 6.));

        // axis aligned boxes
        voxel::edit::axisAlignedBox<t_voxel>::pointer aabEdit(voxel::edit::axisAlignedBox<t_voxel>::create(blub::axisAlignedBox(vector3(-2.), vector3(2.))));

        voxelContainer->editVoxel(aabEdit, transform(vector3(0., 0., 15.)));
        voxelContainer->editVoxel(aabEdit, transform(vector3(10., 0., 15.), quaternion(), 2.));
        voxelContainer->editVoxel(aabEdit, transform(vector3(20., 0., 15.), quaternion(), 4.));
        voxelContainer->editVoxel(aabEdit, transform(vector3(35., 0., 15.), quaternion(), 8.));

//        // boxes
//        quaternion rotation;
//        rotation = rotation * quaternion(math::sin(0.50/2.), 0., 0., math::cos(0.5/2.));
//        rotation = rotation * quaternion(0., math::sin(0.75/2.), 0., math::cos(0.5/2.));
//        rotation = rotation * quaternion(0., 0., math::sin(0.25/2.), math::cos(0.5/2.));
//        voxel::edit::box<t_voxel>::pointer boxEdit(voxel::edit::box<t_voxel>::create(2., rotation));

//        voxelContainer->editVoxel(boxEdit, transform(vector3(0.,  0., 30.)));
//        voxelContainer->editVoxel(boxEdit, transform(vector3(10., 0., 30.), quaternion(), 2.));
//        voxelContainer->editVoxel(boxEdit, transform(vector3(20., 0., 30.), quaternion(), 4.));
//        voxelContainer->editVoxel(boxEdit, transform(vector3(35., 0., 30.), quaternion(), 8.));
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
    voxel::edit::sphere<t_voxel>::pointer sphereEdit(voxel::edit::sphere<t_voxel>::create(sphere(position, 5.)));
    sphereEdit->setCut(cut);
    container->editVoxel(sphereEdit);
}



