#include "main.h"

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "../utils/DotSceneLoader.h"
#include "../utils/BulletXML.h"

#include <iostream>

using namespace std;

bool Test::go()
{
   #ifdef _DEBUG
   mResourcesCfg = "resources_d.cfg";
   mPluginsCfg = "plugins_d.cfg";
   #else
   mResourcesCfg = "resources.cfg";
   mPluginsCfg = "plugins.cfg";
   #endif

   //Create Root
   mRoot = new Ogre::Root(mPluginsCfg);
  
   //Parse resources.cfg
   Ogre::ConfigFile cf;
   cf.load(mResourcesCfg);
   Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
   Ogre::String secName, typeName, archName;
   while (seci.hasMoreElements())
   {
      secName = seci.peekNextKey();
      Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
      Ogre::ConfigFile::SettingsMultiMap::iterator i;
      for (i = settings->begin(); i != settings->end(); ++i)    
      {
	 typeName = i->first;
	 archName = i->second;
	 Ogre::ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
      }
   }
   

   //Create ogre.cfg
   if(!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
   {
      return false;
   }

   //Create window
   mWindow = mRoot->initialise(true, "BasicTutorial6 Render Window");
   
   //Load textures
   Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
   Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    
   //Create Scene Manager
   mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");
   mSceneMgr->setSkyDome(true,"SkyDome");
  
   //Create Camera
   mCamera = mSceneMgr->createCamera("PlayerCam");
   mCamera->setNearClipDistance(5);

   //Create Viewport
   Ogre::Viewport* vp = mWindow->addViewport(mCamera);
   vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
   mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

   //Create Scene looks
   Ogre::Entity* ogreHead = mSceneMgr->createEntity("", "Cone.mesh");
   
   mConeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
   mConeNode->attachObject(ogreHead);
   mConeNode->setScale(5,5,5);
   
   
   mPlayerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
   mPlayerNode->setPosition(0,20,0);
   
   mCameraYawNode = mPlayerNode->createChildSceneNode();
   mCameraPitchNode = mCameraYawNode->createChildSceneNode();
   mCameraPitchNode->attachObject(mCamera);

   // Create a light
   Ogre::Light* l = mSceneMgr->createLight("MainLight");
   l->setPosition(20,80,50);

   //Create OIS 
   Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
   OIS::ParamList pl;
   size_t windowHnd = 0;
   std::ostringstream windowHndStr;
   mWindow->getCustomAttribute("WINDOW", &windowHnd);
   windowHndStr << windowHnd;
   pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
   //pl.insert(std::make_pair(std::string("x11_mouse_grab"), "false"));
   //pl.insert(std::make_pair(std::string("x11_mouse_hide"), "false"));
   pl.insert(std::make_pair(std::string("x11_keyboard_grab"), "false"));
     
   mInputManager = OIS::InputManager::createInputSystem( pl );
   
   mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, false ));
   mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, false ));
   windowResized(mWindow);

   //Add this class as a listener
   Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
   mRoot->addFrameListener(this);

   //Show Overlay
   mOverlayManager = Ogre::OverlayManager::getSingletonPtr();

   mFirstOverlay = mOverlayManager->getByName("Test");
   mFirstOverlay->show();

   mDebugOverlay = mOverlayManager->getByName("Core/DebugOverlay");
   mDebugOverlay->show();
   
   //Load scene
   DotSceneLoader loader;
   loader.parseDotScene("lol.scene","General", mSceneMgr,&mMeshes);
   
   //Activate physics
   loadPhx(); 
   BulletXML test("lol.xml",mWorld,mSceneMgr,&mCopyData,&mMeshes,&mStore);
   test.parse();

   //Need to see stuff
   mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
   
   mPlayerNode->setPosition(0,20,0);
   mRoot->startRendering();
   
   return true;
}


Test::Test() : mRoot(0), mPluginsCfg(Ogre::StringUtil::BLANK) 
{
}

Test::~Test()
{
   Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
   windowClosed(mWindow);
   delete mRoot;
}

bool Test::movePlayer(Ogre::Real time)
{

   Ogre::Vector3 moveVec(0,0,0);

   mKeyboard->capture();
   mMouse->capture();

   OIS::MouseState state = mMouse->getMouseState();
   
   Ogre::Quaternion test(Ogre::Degree(state.X.rel * time * -20),Ogre::Vector3::UNIT_Y);
   Ogre::Quaternion test2(Ogre::Degree(state.Y.rel * time * -20),Ogre::Vector3::UNIT_X);

   moveQuat = moveQuat * test;
   moveQuat.normalise();

   cameraQuat = cameraQuat * test2;
   cameraQuat.normalise();
   
   mCameraYawNode->rotate(test);
   mCameraPitchNode->rotate(test2);
   

   if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
      return false;

   if (mKeyboard->isKeyDown(OIS::KC_A))
      moveVec.x += -20;

   if (mKeyboard->isKeyDown(OIS::KC_S))
      moveVec.x += 20;
   
   if (mKeyboard->isKeyDown(OIS::KC_W))
      moveVec.z += -20;
   
   if (mKeyboard->isKeyDown(OIS::KC_R))
      moveVec.z += 20;

   if(mKeyboard->isKeyDown(OIS::KC_SPACE))
      moveVec.y += 20;

   if(mKeyboard->isKeyDown(OIS::KC_F))
      moveVec.y += -20;
   
   mPlayerNode->translate(moveQuat * (moveVec * time));
   
   return true;
}
  
void Test::updateStats(void)
{
   static Ogre::String currFps = "Current FPS: ";
   static Ogre::String avgFps = "Average FPS: ";
   static Ogre::String bestFps = "Best FPS: ";
   static Ogre::String worstFps = "Worst FPS: ";
   static Ogre::String tris = "Triangle Count: ";
   static Ogre::String batches = "Batch Count: ";

   // update stats when necessary
   try {
      Ogre::OverlayElement* guiAvg = mOverlayManager->getOverlayElement("Core/AverageFps");
      Ogre::OverlayElement* guiCurr = mOverlayManager->getOverlayElement("Core/CurrFps");
      Ogre::OverlayElement* guiBest = mOverlayManager->getOverlayElement("Core/BestFps");
      Ogre::OverlayElement* guiWorst = mOverlayManager->getOverlayElement("Core/WorstFps");

      const Ogre::RenderTarget::FrameStats& stats = mWindow->getStatistics();
      guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
      guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
      guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS) +" "+Ogre::StringConverter::toString(stats.bestFrameTime)+" ms");
      guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(stats.worstFPS) +" "+Ogre::StringConverter::toString(stats.worstFrameTime)+" ms");

      Ogre::OverlayElement* guiTris = mOverlayManager->getOverlayElement("Core/NumTris");
      guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

      Ogre::OverlayElement* guiBatches = mOverlayManager->getOverlayElement("Core/NumBatches");
      guiBatches->setCaption(batches + Ogre::StringConverter::toString(stats.batchCount));

      Ogre::OverlayElement* guiDbg = mOverlayManager->getOverlayElement("Core/DebugText");
      guiDbg->setCaption("Debug something here");
   }
   catch(...) { /* ignore */ }
}

bool Test::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
   Ogre::Real time = evt.timeSinceLastFrame;

   if (mWindow->isClosed())
      return false;

   if (!movePlayer(time))
      return false;
      
   updateStats();
   return true;
}

bool Test::frameStarted(const Ogre::FrameEvent &evt)
{

   if (mKeyboard->isKeyDown(OIS::KC_D))
   {
      Ogre::Vector3 Loc = mPlayerNode->getPosition();
      btVector3 from = BtOgre::Convert::toBullet(mPlayerNode->convertLocalToWorldPosition(Ogre::Vector3(0,0,0)));
      btVector3 to = BtOgre::Convert::toBullet(mPlayerNode->convertLocalToWorldPosition(moveQuat * cameraQuat * Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_Y)* Ogre::Vector3(-100,0,0)));
      //to.normalize();
     
      mConeNode->setPosition(mPlayerNode->convertLocalToWorldPosition(moveQuat * cameraQuat * Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_Y)* Ogre::Vector3(-100,0,0)));
      
       

      RayCall lol(from,to);
      mWorld->rayTest(from,to,lol);
   }
   
   if (mKeyboard->isKeyDown(OIS::KC_T))
   {
      mStore["Box"].body->activate();
      mStore["Box"].body->applyCentralImpulse(btVector3(0,.25,.1));
   }

   if (mKeyboard->isKeyDown(OIS::KC_B))
      cout<<mCamera->getRealPosition().x<<' '<<mCamera->getRealPosition().z<<endl;
   
   mWorld->stepSimulation(evt.timeSinceLastFrame, 10);
   mWorld->debugDrawWorld();

   dbgdraw->setDebugMode(1);
   dbgdraw->step();
   return true;
}

void Test::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

}
 
//Unattach OIS before window shutdown (very important under Linux)
void Test::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void Test::loadPhx()
{
   mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
   mCollisionConfig = new btDefaultCollisionConfiguration();
   mDispatcher = new btCollisionDispatcher(mCollisionConfig);
   mSolver = new btSequentialImpulseConstraintSolver();

   mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
   mWorld->setGravity(btVector3(0,-9.8,0));

   dbgdraw = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), mWorld);
   mWorld->setDebugDrawer(dbgdraw);
}