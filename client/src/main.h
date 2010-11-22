
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "../utils/BtOgrePG.h"
#include "../utils/BtOgreGP.h"
#include "../utils/BtOgreExtras.h"
#include <map>
#include <string>

#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wshadow"

class Test : public Ogre::WindowEventListener,  public Ogre::FrameListener
{
public:
   Test();
   ~Test();
   bool go();

   void updateStats();
   bool movePlayer(Ogre::Real time);
   void loadPhx();
   void addCylinder(const char* name,btCollisionShape *mClyShape,btRigidBody *mClyBody,float x, float z,float wheelx);

protected:
   virtual void windowResized(Ogre::RenderWindow* rw);
   virtual void windowClosed(Ogre::RenderWindow* rw);
   virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
   virtual bool frameStarted(const Ogre::FrameEvent& evt);

private:
   Ogre::Root *mRoot;
   Ogre::String mPluginsCfg;
   Ogre::String mResourcesCfg;
   Ogre::RenderWindow* mWindow;
   Ogre::SceneManager* mSceneMgr;
   Ogre::Camera* mCamera;

   OIS::InputManager* mInputManager;
   OIS::Mouse*    mMouse;
   OIS::Keyboard* mKeyboard;

   Ogre::SceneNode *mConeNode;
   
   Ogre::SceneNode *mCameraYawNode;
   Ogre::SceneNode *mCameraPitchNode;
   Ogre::SceneNode *mPlayerNode;
   Ogre::Quaternion cameraQuat;
   Ogre::Quaternion moveQuat;   

   Ogre::OverlayManager *mOverlayManager;

   Ogre::Overlay *mFirstOverlay;
   Ogre::Overlay *mDebugOverlay;

   btDiscreteDynamicsWorld *mWorld;
   BtOgre::DebugDrawer *dbgdraw;

   btAxisSweep3 *mBroadphase;
   btDefaultCollisionConfiguration *mCollisionConfig;
   btCollisionDispatcher *mDispatcher;
   btSequentialImpulseConstraintSolver *mSolver;

   std::map<std::string,btRigidBody *> mBodies;
   btRigidBody *mBoxBody;
   btRigidBody *mPlayerBody;
   btRigidBody *mGroundBody;

   std::map<std::string,btCollisionShape *> mShapes;
   btCollisionShape *mBoxShape;
   btCollisionShape *mPlayerShape;
   btCollisionShape *mGroundShape;
};

class RayCall : public btCollisionWorld::RayResultCallback
   {
   public:
      RayCall(const btVector3&     rayFromWorld,const btVector3&   rayToWorld)
      :m_rayFromWorld(rayFromWorld),
      m_rayToWorld(rayToWorld)
      {
      }

      btVector3 m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
      btVector3 m_rayToWorld;

      btVector3 m_hitNormalWorld;
      btVector3 m_hitPointWorld;
         
      virtual   btScalar        addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
      {

//caller already does the filter on the m_closestHitFraction
         btAssert(rayResult.m_hitFraction <= m_closestHitFraction);
         
         m_closestHitFraction = rayResult.m_hitFraction;

         m_collisionObject = rayResult.m_collisionObject;
         if (normalInWorldSpace)
         {
            m_hitNormalWorld = rayResult.m_hitNormalLocal;
         } else
         {
            ///need to transform normal into worldspace
            m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
         }
         m_hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,rayResult.m_hitFraction);
         
         return 1.f;
      }
   };

