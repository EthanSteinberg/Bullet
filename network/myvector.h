#ifndef  MYVECTOR_H_INCLUDED
#define  MYVECTOR_H_INCLUDED

#include <OGRE/Ogre.h>
#include <btBulletDynamicsCommon.h>

#include <cstdint>

struct myVector3
{
   static const float SHORT_MAX = INT16_MAX;
   static const float RANGE = 5000;

   int16_t x;
   int16_t y;
   int16_t z;

   myVector3()
   {
      x = 0;
      y = 0;
      z = 0;
   };

   myVector3(const Ogre::Vector3 &vec3)
   {
      x = vec3.x/RANGE * SHORT_MAX;
      y = vec3.y/RANGE * SHORT_MAX;
      z = vec3.z/RANGE * SHORT_MAX;
   }

   operator Ogre::Vector3() const
   {
      Ogre::Vector3 vec3;

      vec3.x = x/ SHORT_MAX * RANGE;
      vec3.y = y/ SHORT_MAX * RANGE;
      vec3.z = z/ SHORT_MAX * RANGE;

      return vec3;
   } 

   myVector3(const btVector3 &vec3)
   {
      x = vec3.x()/RANGE * SHORT_MAX;
      y = vec3.y()/RANGE * SHORT_MAX;
      z = vec3.z()/RANGE * SHORT_MAX;
   }

   operator btVector3() const
   {
      btVector3 vec3;

      vec3.setX(x/ SHORT_MAX * RANGE);
      vec3.setY(y/ SHORT_MAX * RANGE);
      vec3.setZ(z/ SHORT_MAX * RANGE);

      return vec3;
   } 
};

#endif
