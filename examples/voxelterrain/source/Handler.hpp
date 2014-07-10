#include "blub/async/dispatcher.hpp"
#include "blub/core/globals.hpp"
#include "blub/core/log.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/core/scopedPtr.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreFrameListener.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreRoot.h>

#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>


/**
 * @brief The Handler class initialises Ogre3d (graphics) and OIS (input) and derives their callbacks.
 * Class shall simplify the examples.
 */
class Handler : public Ogre::FrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
    /**
     * @brief Handler constructor
     */
    Handler()
        : graphicDispatcher(0, true)
        , camera(nullptr)
        , keyboard(nullptr)
        , mouse(nullptr)
        , m_lookVert(blub::math::pi)
        , m_lookHor(0.)
        , m_forward(false)
        , m_backwards(false)
        , m_left(false)
        , m_right(false)
        , m_fast(false)
    {
        ;
    }

    /**
     * @brief ~Handler destructor.
     */
    virtual ~Handler()
    {
        inputManager->destroyInputObject(keyboard);
        inputManager->destroyInputObject(mouse);
        OIS::InputManager::destroyInputSystem(inputManager);

        renderScene->destroyAllMovableObjects();
    }

    /**
     * @brief initialiseOgre initialises ogre3d, including scene and camera.
     * @return Returns false if window for graphic settings got cancelled by user.
     */
    bool initialiseOgre()
    {
        // initalise Ogre3d
        renderSystem.reset(new Ogre::Root());
        Ogre::LogManager::getSingleton().getDefaultLog()->setDebugOutputEnabled(true);
        if (!renderSystem->restoreConfig())
        {
            if (!renderSystem->showConfigDialog())
            {
                return false;
            }
        }
        renderSystem->initialise(true);

        // renderScene = renderSystem->createSceneManager(Ogre::ST_GENERIC);
        renderScene = renderSystem->createSceneManager("OctreeSceneManager");

//        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../data", "FileSystem");
//        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        camera = renderScene->createCamera("camera");
        camera->setAutoAspectRatio(true);
        camera->setNearClipDistance(0.1);
        camera->setFarClipDistance(1000.);
        renderSystem->getAutoCreatedWindow()->addViewport(camera);
        camera->getViewport()->setBackgroundColour(Ogre::ColourValue::Black);

        renderScene->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        Ogre::Light *light = renderScene->createLight();
        light->setType(Ogre::Light::LT_DIRECTIONAL);
        light->setDirection(1., -1., -1.);

        renderSystem->addFrameListener(this);

        return true;
    }

    /**
     * @brief initialiseOIS initialises OIS
     * @return Always true.
     */
    bool initialiseOIS()
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
        //pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
    #endif
        inputManager = OIS::InputManager::createInputSystem(pl);
        keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
        mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));

        keyboard->setEventCallback(this);
        mouse->setEventCallback(this);

        return true;
    }

    // FrameListener interface
public:
    /**
     * @brief frameStarted gets called after every rendered frame by ogre3d.
     * Method calls the graphic dispatcher used by OgreTile and the signal signalFrame().
     * @param evt Contains timeSinceLastFrame
     * @return Returns false if render window got closed.
     */
    bool frameStarted(const Ogre::FrameEvent &evt)
    {
        Ogre::RenderWindow *renderWindow(Ogre::Root::getSingletonPtr()->getAutoCreatedWindow());
        if (renderWindow->isClosed())
        {
            return false;
        }

        keyboard->capture();
        mouse->capture();

        {
            blub::real speed(5.);
            if (m_fast)
            {speed = 20.;}
            if (m_forward)
            {
                camera->moveRelative(Ogre::Vector3::NEGATIVE_UNIT_Z*speed*evt.timeSinceLastFrame);
            }
            if (m_backwards)
            {
                camera->moveRelative(Ogre::Vector3::UNIT_Z*speed*evt.timeSinceLastFrame);
            }
            if (m_left)
            {
                camera->moveRelative(Ogre::Vector3::NEGATIVE_UNIT_X*speed*evt.timeSinceLastFrame);
            }
            if (m_right)
            {
                camera->moveRelative(Ogre::Vector3::UNIT_X*speed*evt.timeSinceLastFrame);
            }
        }

        m_sigFrame(evt.timeSinceLastFrame);
        graphicDispatcher.start();

        return true;
    }

    // MouseListener interface
public:
    /**
     * @brief mouseMoved gets called by OIS. Calculates the camera orientation and sets the results.
     * @param arg
     * @return
     */
    bool mouseMoved(const OIS::MouseEvent &arg)
    {
        m_lookHor += static_cast<blub::real>(arg.state.Y.rel) / -500.;
        m_lookVert += static_cast<blub::real>(arg.state.X.rel) / -500.;

        m_lookHor = blub::math::clamp<blub::real>(m_lookHor, -M_PI_2, M_PI_2);

        const blub::quaternion rotVert(0, blub::math::sin(m_lookVert / 2.0), 0, blub::math::cos(m_lookVert / 2.0));
        const blub::quaternion rotHor(blub::math::sin(m_lookHor / 2.0), 0, 0, blub::math::cos(m_lookHor / 2.0));

        blub::quaternion rot(rotVert*rotHor);

        camera->setOrientation(rot);

        return true;
    }

    /**
     * @brief mousePressed Checks if mouse button "left" or "right" got pressed and calls the signal signalKeyGotPressed()
     * @param arg
     * @return
     */
    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID /*id*/)
    {
        if (!arg.state.buttonDown(OIS::MB_Left) && !arg.state.buttonDown(OIS::MB_Right))
        {return true;}

        m_sigMouseGotPressed(arg.state.buttonDown(OIS::MB_Left));

        return true;
    }
    /**
     * @brief mouseReleased Does nothing. Must get implemented because pure virtual in OIS::KeyListener
     * @return
     */
    bool mouseReleased(const OIS::MouseEvent &/*arg*/, OIS::MouseButtonID /*id*/)
    {
        return true;
    }

    // KeyListener interface
public:

    /**
     * @brief keyPressed gets calles by OIS. If key F11 got pressed rendering mode gets changed to wireframe.
     * @param arg
     * @return
     */
    bool keyPressed(const OIS::KeyEvent &arg)
    {
        if (arg.key == OIS::KC_F11)
        {
            if (camera->getPolygonMode() == Ogre::PM_WIREFRAME)
            {
                camera->setPolygonMode(Ogre::PM_SOLID);
            }
            else
            {
                camera->setPolygonMode(Ogre::PM_WIREFRAME);
            }
            return true;
        }
        return handleKeyPress(arg, true);
    }
    /**
     * @brief keyReleased Does nothing. Must get implemented because pure virtual in OIS::KeyListener
     * @param arg
     * @return
     */
    bool keyReleased(const OIS::KeyEvent &arg)
    {
        return handleKeyPress(arg, false);
    }
    /**
     * @brief handleKeyPress checks if WSAD or LShift gets pressed and moves the camera.
     * @param arg
     * @param pressed
     * @return
     */
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

        m_sigKeyGotPressed(arg, pressed);

        return true;
    }

    typedef blub::signal<void (blub::real)> t_sigFrame;
    t_sigFrame* signalFrame() {return &m_sigFrame;}
    typedef blub::signal<void (OIS::KeyEvent, bool pressed)> t_sigKeyGotPressed;
    t_sigKeyGotPressed* signalKeyGotPressed() {return &m_sigKeyGotPressed;}
    typedef blub::signal<void (bool left)> t_sigMouseGotPressed;
    t_sigMouseGotPressed* signalMouseGotPressed() {return &m_sigMouseGotPressed;}

public:
    blub::async::dispatcher graphicDispatcher;

    blub::scopedPointer<Ogre::Root> renderSystem;
    Ogre::SceneManager* renderScene;
    Ogre::Camera* camera;

    OIS::InputManager *inputManager;
    OIS::Keyboard *keyboard;
    OIS::Mouse *mouse;

protected:
    blub::real m_lookVert;
    blub::real m_lookHor;
    bool m_forward;
    bool m_backwards;
    bool m_left;
    bool m_right;
    bool m_fast;

    t_sigFrame m_sigFrame;
    t_sigKeyGotPressed m_sigKeyGotPressed;
    t_sigMouseGotPressed m_sigMouseGotPressed;
};
