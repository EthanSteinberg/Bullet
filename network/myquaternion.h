#ifndef  MYQUATERNION_H_INCLUDED
#define  MYQUATERNION_H_INCLUDED

#include <OGRE/Ogre.h>
#include <btBulletDynamicsCommon.h>

#include <cstdint>

struct myQuaternion
{
   static const float SHORT_MAX = INT16_MAX;
   static const float RANGE = 5000;

   int16_t x;
   int16_t y;
   int16_t z;
   int16_t w;

   myQuaternion()
   {
      x = 0;
      y = 0;
      z = 0;
      w = 0;
   };

   myQuaternion(const Ogre::Quaternion &quat)
   {
      x = quat.x/RANGE * SHORT_MAX;
      y = quat.y/RANGE * SHORT_MAX;
      z = quat.z/RANGE * SHORT_MAX;
      w = quat.w/RANGE * SHORT_MAX;
   }

   operator Ogre::Quaternion() const
   {
      Ogre::Quaternion quat;

      quat.x = x/ SHORT_MAX * RANGE;
      quat.y = y/ SHORT_MAX * RANGE;
      quat.z = z/ SHORT_MAX * RANGE;
      quat.w = w/ SHORT_MAX * RANGE;

      return quat;
   } 

   myQuaternion(const btQuaternion &quat)
   {
      x = quat.x()/RANGE * SHORT_MAX;
      y = quat.y()/RANGE * SHORT_MAX;
      z = quat.z()/RANGE * SHORT_MAX;
      w = quat.w()/RANGE * SHORT_MAX;
   }

   operator btQuaternion() const
   {
      btQuaternion quat;

      quat.setX(x/ SHORT_MAX * RANGE);
      quat.setY(y/ SHORT_MAX * RANGE);
      quat.setZ(z/ SHORT_MAX * RANGE);
      quat.setW(w/ SHORT_MAX * RANGE);

      return quat;
   } 
};

#endif
