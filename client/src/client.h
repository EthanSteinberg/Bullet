#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "../utils/BtOgrePG.h"
#include "../network/network.h"
#include "../utils/BtOgreGP.h"
#include "../utils/BtOgreExtras.h"
#include "../utils/BulletXML.h"

#include <map>
#include <queue>
#include <set>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>


class Client : public Ogre::WindowEventListener,  public Ogre::FrameListener, public OIS::KeyListener
{
public:
   Client();
   ~Client();
   bool go();
   void start(std::string);
   void startGame();

protected:
   virtual void windowResized(Ogre::RenderWindow* rw);
   virtual void windowClosed(Ogre::RenderWindow* rw);
   virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
   virtual bool frameStarted(const Ogre::FrameEvent& evt);

   virtual bool keyPressed( const OIS::KeyEvent &arg);
   virtual bool keyReleased( const OIS::KeyEvent &arg);  

private: 
   void timeout(const boost::system::error_code &error, boost::asio::deadline_timer &timer, boost::asio::ip::udp::socket *sock, void *packet,std::size_t size);
  
   void handler(const boost::system::error_code &error, std::size_t bytes_transferred);
   void gameHandler(const boost::system::error_code &error, std::size_t bytes_transferred);
   //void pingtimeout(const boost::system::error_code &error,boost::asio::deadline_timer &timer, udp::socket &sock,t_pingPacket &pingPacket);
   //void connecttimeout(const boost::system::error_code &error,boost::asio::deadline_timer &timer, udp::socket &sock,t_connectPacket &connectPacket);

   boost::asio::ip::udp::socket *sock;
   boost::asio::io_service ioserv;
   boost::asio::ip::udp::endpoint *end;
   
   int recieved;
   uint8_t ReceiveBuffer[512];
   std::set<uint16_t> objectsLeft;
 

   void updateStats();
   bool movePlayer(Ogre::Real time);
   void loadPhx();
   void parseObject(const t_objectData &ObjectData);
   void parseObjectData();

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

   //std::map<std::string,btRigidBody *> mBodies;
   //btRigidBody *mBoxBody;
   //btRigidBody *mPlayerBody;
   //btRigidBody *mGroundBody;

   //std::map<std::string,btCollisionShape *> mShapes;
   //btCollisionShape *mBoxShape;
   //btCollisionShape *mPlayerShape;
   //btCollisionShape *mGroundShape;

   std::map<std::string/*entity name*/, std::string/*meshlocation*/> mMeshes;  

   std::map<std::string, t_Store> mStore;
   std::map<uint16_t, t_objectData> mObjectData;

   std::queue<t_updatePacket> mUpdateQueue;
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

