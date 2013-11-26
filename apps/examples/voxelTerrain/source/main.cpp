#include "blub/core/log.hpp"
#include "blub/math/math.hpp"
#include "blub/math/quaternion.hpp"
#include "blub/math/sphere.hpp"
#include "blub/math/transform.hpp"
#include "blub/sync/identifier.hpp"
#include "blub/procedural/voxel/edit/sphere.hpp"
#include "blub/procedural/voxel/simple/container/inMemory.hpp"
#include "blub/procedural/voxel/terrain/accessor.hpp"
#include "blub/procedural/voxel/terrain/surface.hpp"
#include "blub/procedural/voxel/terrain/renderer.hpp"
#include "blub/procedural/voxel/tile/container.hpp"

#include "OgreTile.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreRoot.h>

#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <boost/lexical_cast.hpp>


typedef blub::sharedPointer<blub::sync::identifier> t_cameraIdentifier;


class Handler : public Ogre::FrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
    Handler(blub::async::dispatcher &graphicDispatcher,
            blub::procedural::voxel::simple::container::base& voxelContainer,
            blub::procedural::voxel::terrain::renderer& voxelRenderer,
            t_cameraIdentifier cameraIdentifier,
            Ogre::Camera* camera,
            OIS::Keyboard* keyboard,
            OIS::Mouse* mouse)
        : m_graphicDispatcher(graphicDispatcher)
        , m_voxelContainer(voxelContainer)
        , m_voxelRenderer(voxelRenderer)
        , m_cameraIdentifier(cameraIdentifier)
        , m_camera(camera)
        , m_keyboard(keyboard)
        , m_mouse(mouse)
        , m_lookVert(0.)
        , m_lookHor(0.)
        , m_forward(false)
        , m_backwards(false)
        , m_fast(false)
    {
        m_edit = blub::procedural::voxel::edit::sphere::create(blub::sphere(blub::vector3(), 5.));
    }

    // FrameListener interface
public:
    bool frameStarted(const Ogre::FrameEvent &evt)
    {
        if (Ogre::Root::getSingletonPtr()->getAutoCreatedWindow()->isClosed())
        {
            return false;
        }

        m_keyboard->capture();
        m_mouse->capture();

        {
            blub::real speed(5.);
            if (m_fast)
            {speed = 20.;}
            if (m_forward)
            {
                m_camera->moveRelative(Ogre::Vector3::NEGATIVE_UNIT_Z*speed*evt.timeSinceLastFrame);
            }
            if (m_backwards)
            {
                m_camera->moveRelative(Ogre::Vector3::UNIT_Z*speed*evt.timeSinceLastFrame);
            }
            if (m_left)
            {
                m_camera->moveRelative(Ogre::Vector3::NEGATIVE_UNIT_X*speed*evt.timeSinceLastFrame);
            }
            if (m_right)
            {
                m_camera->moveRelative(Ogre::Vector3::UNIT_X*speed*evt.timeSinceLastFrame);
            }
        }

        m_voxelRenderer.updateCamera(m_cameraIdentifier, m_camera->getPosition());
        m_graphicDispatcher.start();

        return true;
    }

    // MouseListener interface
public:
    bool mouseMoved(const OIS::MouseEvent &arg)
    {
        m_lookHor += static_cast<blub::real>(arg.state.Y.rel) / -500.;
        m_lookVert += static_cast<blub::real>(arg.state.X.rel) / -500.;

        m_lookHor = blub::math::saturate(m_lookHor, -M_PI_2, M_PI_2);

        const blub::quaternion rotVert(0, blub::math::sin(m_lookVert / 2.0), 0, blub::math::cos(m_lookVert / 2.0));
        const blub::quaternion rotHor(blub::math::sin(m_lookHor / 2.0), 0, 0, blub::math::cos(m_lookHor / 2.0));

        blub::quaternion rot(rotVert*rotHor);

        m_camera->setOrientation(rot);

        return true;
    }
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        if (!arg.state.buttonDown(OIS::MB_Left) && !arg.state.buttonDown(OIS::MB_Right))
        {return true;}

        m_edit->setCut(arg.state.buttonDown(OIS::MB_Right));
        m_voxelContainer.editVoxel(m_edit.staticCast<blub::procedural::voxel::edit::base>(), blub::transform(m_camera->getPosition() + m_camera->getDirection()*10.));
        return true;
    }
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        return true;
    }

    // KeyListener interface
public:
    bool keyPressed(const OIS::KeyEvent &arg)
    {
        if (arg.key >= OIS::KC_1 && arg.key <= OIS::KC_0)
        {
            m_edit = blub::procedural::voxel::edit::sphere::create(blub::sphere(blub::vector3(), static_cast<blub::int32>(arg.key-OIS::KC_1)+1));
        }
        if (arg.key == OIS::KC_F11)
        {
            if (m_camera->getPolygonMode() == Ogre::PM_WIREFRAME)
            {
                m_camera->setPolygonMode(Ogre::PM_SOLID);
            }
            else
            {
                m_camera->setPolygonMode(Ogre::PM_WIREFRAME);
            }
        }
        return handleKeyPress(arg, true);
    }
    bool keyReleased(const OIS::KeyEvent &arg)
    {
        return handleKeyPress(arg, false);
    }
    bool handleKeyPress(const OIS::KeyEvent &arg, const bool& pressed)
    {
        if (arg.key == OIS::KC_W)
        {m_forward = pressed;}
        if (arg.key == OIS::KC_S)
        {m_backwards = pressed;}
        if (arg.key == OIS::KC_A)
        {m_left = pressed;}
        if (arg.key == OIS::KC_D)
        {m_right = pressed;}
        if (arg.key == OIS::KC_LSHIFT)
        {m_fast = pressed;}
        return true;
    }

private:
    blub::async::dispatcher &m_graphicDispatcher;
    blub::procedural::voxel::simple::container::base &m_voxelContainer;
    blub::procedural::voxel::terrain::renderer &m_voxelRenderer;
    t_cameraIdentifier m_cameraIdentifier;

    blub::sharedPointer<blub::procedural::voxel::edit::sphere> m_edit;

    Ogre::Camera *m_camera;

    OIS::Keyboard *m_keyboard;
    OIS::Mouse *m_mouse;

    blub::real m_lookVert;
    blub::real m_lookHor;
    bool m_forward;
    bool m_backwards;
    bool m_left;
    bool m_right;
    bool m_fast;
};



int main(int /*argc*/, char */*argv*/[])
{   
    Ogre::Root* renderSystem(nullptr);
    Ogre::SceneManager* renderScene(nullptr);
    Ogre::SceneNode *renderNode(nullptr);
    Ogre::Camera* camera(nullptr);
    Handler *handler(nullptr);

    OIS::InputManager *inputManager(nullptr);
    OIS::Keyboard *keyboard(nullptr);
    OIS::Mouse *mouse(nullptr);

    blub::procedural::voxel::simple::container::inMemory *voxelContainer(nullptr);
    blub::procedural::voxel::terrain::accessor *voxelAccessor(nullptr);
    blub::procedural::voxel::terrain::surface *voxelSurface(nullptr);
    blub::procedural::voxel::terrain::renderer *voxelRenderer(nullptr);

    blub::log::initialise("blub.log");
    blub::async::dispatcher worker(4, false, "terrain");
    blub::async::dispatcher graphicDispatcher(0, true, "graphic");

    t_cameraIdentifier cameraIdentifier;

    // renderer
    {
        // initalise Ogre3d
        renderSystem = new Ogre::Root();
        Ogre::LogManager::getSingleton().getDefaultLog()->setDebugOutputEnabled(true);
        if (!renderSystem->restoreConfig())
        {
            renderSystem->showConfigDialog();
        }
        renderSystem->initialise(true);

        renderScene = renderSystem->createSceneManager(Ogre::ST_GENERIC);

        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../data", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        camera = renderScene->createCamera("camera");
        camera->setAutoAspectRatio(true);
        camera->setNearClipDistance(0.1);
        camera->setFarClipDistance(1000.);
        renderSystem->getAutoCreatedWindow()->addViewport(camera);
        camera->getViewport()->setBackgroundColour(Ogre::ColourValue::Black);

        renderNode = renderScene->getRootSceneNode();
    }

    // input
    {
        OIS::ParamList pl;
        size_t windowHandle = 0;

        renderSystem->getAutoCreatedWindow()->getCustomAttribute("WINDOW", &windowHandle);

        pl.insert(std::make_pair(std::string("WINDOW"), boost::lexical_cast<std::string>(windowHandle)));
    #if defined OIS_WIN32_PLATFORM
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_BACKGROUND" )));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_EXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    #elif defined OIS_LINUX_PLATFORM
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("true")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
    #endif
        inputManager = OIS::InputManager::createInputSystem(pl);
        keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
        mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
    }

    // initialise terrain
    {
        const blub::int32 numLod(3);

        using namespace blub::procedural::voxel;
        // voxel themself
        voxelContainer = new simple::container::inMemory(worker, 1.);

        // accessor
        voxelAccessor = new terrain::accessor(worker, *voxelContainer, numLod);

        // surface
        voxelSurface = new terrain::surface(worker, *voxelAccessor, 1.);

        // ogre3d render wrapper
        auto callbackCreate = boost::bind(OgreTile::create, renderScene, "triplanar", &graphicDispatcher, _1, _2);

        // renderer
        terrain::renderer::t_syncRadiusList lodRadien(numLod);
        lodRadien[0] = tile::container::voxelLength*2.0;
        lodRadien[1] = tile::container::voxelLength*4.0;
        lodRadien[2] = tile::container::voxelLength*6.0;
        voxelRenderer = new terrain::renderer(worker, *voxelSurface, callbackCreate, lodRadien, 1.);
        cameraIdentifier = new blub::sync::identifier();
        voxelRenderer->addCamera(cameraIdentifier, camera->getPosition());
    }

    // create voxel
    {
        using namespace blub::procedural::voxel;
        edit::base::pointer simpleSphere(edit::sphere::create(blub::sphere(blub::vector3(), 5.)));

        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(0, 0, -10)));
        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(0, 0, 10)));
        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(-10, 0, 0)));
        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(10, 0, 0)));
        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(0, -10, 0)));
        voxelContainer->editVoxel(simpleSphere, blub::transform(blub::vector3(0, 10, 0)));
    }

    {
        handler = new Handler(graphicDispatcher,
                              *voxelContainer,
                              *voxelRenderer,
                              cameraIdentifier,
                              camera,
                              keyboard,
                              mouse);
        renderSystem->addFrameListener(handler);
        keyboard->setEventCallback(handler);
        mouse->setEventCallback(handler);
    }

    // start rendering and calling method RenderHandler::frameStarted
    {
        worker.start();
        renderSystem->startRendering();
        worker.stop();
    }

    // shut down
    {
        inputManager->destroyInputObject(keyboard);
        inputManager->destroyInputObject(mouse);
        OIS::InputManager::destroyInputSystem(inputManager);

        renderScene->destroyAllMovableObjects();
        delete renderSystem;

        delete voxelRenderer;
        delete voxelSurface;
        delete voxelAccessor;
        delete voxelContainer;

        delete handler;
    }

    return EXIT_SUCCESS;
}



