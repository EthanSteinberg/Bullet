#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "boost/date_time/posix_time/posix_time.hpp"

#include <OGRE/Ogre.h>
#include <btBulletDynamicsCommon.h>

#include <cstdint>

struct t_Packet
{
   uint8_t type;

   t_Packet()
   {
      type = 0;
   }
};

struct t_pingPacket : public t_Packet
{
   int64_t time;

   t_pingPacket()
   {
      type = 1;
   }
};

struct t_connectPacket : public t_Packet
{
   int64_t id;
   
   t_connectPacket()
   {
      type = 2;
   }
};

struct t_worldPacket : public t_Packet
{
   int64_t numOfObjects;
   
   t_worldPacket()
   {
      type = 3;
   }
};

struct myVector3
{
   static const float SHORT_MAX = INT16_MAX;
   static const float RANGE = 100;

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

struct t_objectData
{
   char name[20];

   char sceneName[20];
   char entName[20];
   char meshName[20];

   myVector3 position;
   myVector3 scale;

   myVector3 linearVelocity;
   myVector3 angularVelocity;
   //Ogre::Vector3 position;
   Ogre::Quaternion orientation;
   
   //btVector3 totalForce;
   //btVector3 totalTorque;

   uint16_t mass;
   uint16_t friction;
   uint8_t type;
};

//NEED TO SET TYPE YOURSELF
//Type = 4
struct t_objectPacket : public t_Packet 
{
   int64_t numOfObjects;

   uint16_t numbers[4];
   
   t_objectData objectData[];
};


struct t_keyPacket : public t_Packet
{
   // 1 for pressed, 0 for released
   bool keyReleased;

   uint16_t keyCode;
   
   t_keyPacket()
   {
      type = 5;
   }
};
#endif
