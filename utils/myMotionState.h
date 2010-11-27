
#include "BtOgrePG.h"

class myMotionState : public btOgre::RigidBodyState 
{
public:
        myMotionState(Ogre::SceneNode *node, const btTransform &transform, const btTransform &offset = btTransform::getIdentity())
            : mTransform(transform),
              mCenterOfMassOffset(offset),
              mNode(node)
        {
        }

         myMotionState(Ogre::SceneNode *node)
            : mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0,0,0,1)), 
                         ((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition())    : btVector3(0,0,0))),
              mCenterOfMassOffset(btTransform::getIdentity()),
              mNode(node)
        {
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
        }
};
