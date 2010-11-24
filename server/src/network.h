#include "boost/date_time/posix_time/posix_time.hpp"

#include <OGRE/Ogre.h>
#include <btBulletDynamicsCommon.h>

#include <cstdint>
#include <boost/bimap.hpp>
#include <boost/asio.hpp>

typedef boost::bimap<boost::bimaps::set_of<int>, boost::bimaps::set_of<boost::asio::ip::udp::endpoint> > bm;

class server
{
public:
   void start(void);

private:
   void serverHandler(const boost::system::error_code &error, std::size_t bytes_transferred);
   
   bm table;
   boost::asio::ip::udp::endpoint *end;  
   boost::asio::ip::udp::socket *sock;

   uint8_t ReceiveBuffer[512];
   int CurId;
};

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

   operator Ogre::Vector3()
   {
      Ogre::Vector3 vec3;

      vec3.x = x/ SHORT_MAX * RANGE;
      vec3.y = y/ SHORT_MAX * RANGE;
      vec3.z = z/ SHORT_MAX * RANGE;

      return vec3;
   } 
};

struct t_objectData
{
   char name[20];

   char nodeName[20];
   char entName[20];
   char meshLocation[20];

   myVector3 position;
   myVector3 orientation;
   myVector3 totalForce;
   myVector3 totalTorque;
   //Ogre::Vector3 position;
   //Ogre::Quaternion orientation;
   
   //btVector3 totalForce;
   //btVector3 totalTorque;

   uint16_t mass;
   uint16_t friction;
   uint8_t type;
};

//NEED TO SET TYPE YOURSELF
struct t_objectPacket : public t_Packet 
{
   int64_t numOfObjects;

   uint16_t numbers[4];
   
   t_objectData objectData[];
};
