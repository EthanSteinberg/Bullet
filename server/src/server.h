#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "../utils/BtOgrePG.h"
#include "../utils/BtOgreGP.h"
#include "../utils/BtOgreExtras.h"
#include "../utils/BulletXML.h"
#include <map>
#include <string>

#include "../network/network.h"
#include <boost/bimap.hpp>
#include <boost/asio.hpp>

typedef boost::bimap<boost::bimaps::set_of<int>, boost::bimaps::set_of<boost::asio::ip::udp::endpoint> > bm;

class Server : public Ogre::WindowEventListener,  public Ogre::FrameListener
{
public:
   Server();
   ~Server();
   bool go();
   void start(void);
   void run();

   void updateStats();
   bool movePlayer(Ogre::Real time);
   void loadPhx();

   void sendUpdate(std::string,btVector3,btQuaternion)
   {}

protected:
   virtual void windowResized(Ogre::RenderWindow* rw);
   virtual void windowClosed(Ogre::RenderWindow* rw);
   virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
   virtual bool frameStarted(const Ogre::FrameEvent& evt);

private:
   void serverHandler(const boost::system::error_code &error, std::size_t bytes_transferred);
   void MoveEvents();
   void MoveFunction();
   void StartMoveEvents();
   

   bool boxPush;
   
   uint8_t ReceiveBuffer[512];
   int CurId; 
   
   bm table;
   
   boost::asio::io_service ioserv;
   boost::asio::ip::udp::endpoint *end;  
   boost::asio::ip::udp::socket *sock;
   
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

   std::map<std::string/*entity name*/, std::string/*meshlocation*/> mMeshes;
   
   std::map<uint16_t, t_CopyData> mCopyData;
   std::map<std::string, t_Store> mStore;
};


class myMotionState : public btMotionState 
{
    protected:
        btTransform mTransform;
        btTransform mCenterOfMassOffset;

        Ogre::SceneNode *mNode;
        std::string mName;
        Server *mServer;

    public:
        myMotionState(Ogre::SceneNode *node, const btTransform &transform, const btTransform &offset = btTransform::getIdentity())
            : mTransform(transform),
              mCenterOfMassOffset(offset),
              mNode(node)
        {
        }

        myMotionState(Ogre::SceneNode *node,const std::string &name,Server *server)
            : mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0,0,0,1)), 
                         ((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition())    : btVector3(0,0,0))),
              mCenterOfMassOffset(btTransform::getIdentity()),
              mNode(node),
              mName(name),
              mServer(server)
        {
        }

        virtual void getWorldTransform(btTransform &ret) const 
        {
            ret = mCenterOfMassOffset.inverse() * mTransform;
        }

        virtual void setWorldTransform(const btTransform &in) 
        {
            if (mNode == NULL)
                return;

            mTransform = in;
            btTransform transform = in * mCenterOfMassOffset;

            btQuaternion rot = transform.getRotation();
            btVector3 pos = transform.getOrigin();
            mNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
            mNode->setPosition(pos.x(), pos.y(), pos.z());
            mServer->sendUpdate(mName,pos,rot);
        }

        void setNode(Ogre::SceneNode *node) 
        {
            mNode = node;
        }
};
#endif
