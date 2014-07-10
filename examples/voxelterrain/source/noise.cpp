#include "blub/core/log.hpp"
#include "blub/log/system.hpp"
#include "blub/math/math.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/sphere.hpp"
#include "blub/math/transform.hpp"
#include "blub/sync/identifier.hpp"
#include "blub/procedural/voxel/edit/noise.hpp"
#include "blub/procedural/voxel/edit/sphere.hpp"
#include "blub/procedural/voxel/simple/container/inMemory.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/terrain/surface.hpp"
#include "blub/procedural/voxel/terrain/renderer.hpp"
#include "blub/procedural/voxel/tile/container.hpp"

#include "OgreTile.hpp"
#include "Handler.hpp"


/** @example noise.cpp
 * This is an example creates a randomized figure by using simplex noise.
 * @image html voxel_noise.png
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
    blub::log::system::addConsole();
    blub::log::system::addFile("voxelterrain.log");

    Handler handler;
    if (!handler.initialiseOgre())
    {
        return EXIT_FAILURE;
    }
    if (!handler.initialiseOIS())
    {
        return EXIT_FAILURE;
    }

    // initialise terrain   

    scopedPointer<t_voxelContainer> voxelContainer;
    scopedPointer<t_voxelAccessor> voxelAccessor;
    scopedPointer<t_voxelSurface> voxelSurface;
    scopedPointer<t_voxelRenderer> voxelRenderer;

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
        t_voxelRenderer::t_syncRadiusList lodRadien(numLod);
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
        voxel::edit::noise<t_voxel>::pointer noise(voxel::edit::noise<t_voxel>::create(axisAlignedBox(vector3(-100., -100., -100), vector3(100., 100., 100.)), vector3(0.025)));
        voxelContainer->editVoxel(noise);
    }


    // start rendering and begin terrain worker.
    {
        terrainDispatcher.start();
        handler.renderSystem->startRendering();
        terrainDispatcher.stop();
    }

    return EXIT_SUCCESS;
}


void createSphere(t_voxelContainer *container, const vector3 &position, const bool &cut)
{
    BLUB_LOG_OUT() << "createSphere";

    voxel::edit::sphere<t_voxel>::pointer sphereEdit(voxel::edit::sphere<t_voxel>::create(sphere(position, 5.)));
    sphereEdit->setCut(cut);
    container->editVoxel(sphereEdit);
}



