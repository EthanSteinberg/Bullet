#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "boost/date_time/posix_time/posix_time.hpp"

#include "myquaternion.h"
#include "myvector.h"

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


struct t_objectData
{
   char name[20];

   char sceneName[20];
   char entName[20];
   char meshName[20];

   myVector3 position;
   myQuaternion orientation;
   myVector3 scale;

   myVector3 linearVelocity;
   myVector3 angularVelocity;

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


struct t_eventPacket : public t_Packet
{
   uint16_t eventCode;
   
   t_eventPacket()
   {
      type = 5;
   }
};

struct t_updatePacket : public t_Packet
{
   char name[20];

   myVector3 position;
   myQuaternion orientation;
   
   t_updatePacket()
   {
      type = 6;
   }
};
#endif
