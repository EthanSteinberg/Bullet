#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <cstdint>

#include <OIS/OIS.h>

#include <map>
#include <set>
#include <string>

#include "client.h"
#include "../network/network.h"

using namespace boost::asio::ip;

using std::cout;
using std::endl;

void Client::start(std::string place)
{

   boost::asio::io_service ioserv;
   boost::asio::deadline_timer timer(ioserv);

   sock = new udp::socket(ioserv);
   udp::resolver reso(ioserv);
   udp::resolver::query que(udp::v4(),place,"");
   udp::endpoint end(*reso.resolve(que));
   end.port(1327);
   
   sock->connect(end);
   
   {
      sock->async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(&Client::handler,this,_1,_2));
   
      recieved = 0;

      t_pingPacket pingPacket;
      pingPacket.time = boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds();

      sock->send(boost::asio::buffer(&pingPacket,sizeof(pingPacket)));
   
      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(&Client::timeout,this,boost::asio::placeholders::error,boost::ref(timer),sock,&pingPacket, sizeof(pingPacket)));
   
      ioserv.run();
      ioserv.reset();
   }

   {
      sock->async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(&Client::handler,this,_1,_2));
   
      recieved = 0;
   
      t_connectPacket connectPacket;

      sock->send(boost::asio::buffer(&connectPacket,sizeof(connectPacket)));

      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(&Client::timeout,this,boost::asio::placeholders::error,boost::ref(timer),sock,&connectPacket, sizeof(connectPacket)));
   
      ioserv.run();
      ioserv.reset();
   }
   
   {
      sock->async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(&Client::handler,this,_1,_2));
   
      recieved = 0;
   
      t_worldPacket worldPacket;

      sock->send(boost::asio::buffer(&worldPacket,sizeof(worldPacket)));

      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(&Client::timeout,this,boost::asio::placeholders::error,boost::ref(timer),sock, &worldPacket,sizeof(worldPacket)));
   
      ioserv.run();
      ioserv.reset();
   }

   while (!objectsLeft.empty())
   {
      sock->async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(&Client::handler,this,_1,_2));
      
      recieved = 0;

      t_objectPacket objectPacket;
      objectPacket.type = 4;

      int i =0;
      std::set<uint16_t>::iterator it = objectsLeft.begin();

      for (;i<4 && it != objectsLeft.end();i++,it++)
      {
        objectPacket.numbers[i] = *it;
      }

      objectPacket.numOfObjects = i;
      
      sock->send(boost::asio::buffer(&objectPacket,sizeof(objectPacket)));
      
      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(&Client::timeout,this,boost::asio::placeholders::error,boost::ref(timer),sock, &objectPacket, sizeof(objectPacket)));
      
      ioserv.run();
      ioserv.reset();
   }
}

void Client::handler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
   switch(reinterpret_cast<t_Packet *>(ReceiveBuffer)->type)
   {
      
      case 0:
      {
         cout<<"Got an empty packet"<<endl;
      }
         break;
      
      case 1: 
      {
         cout<<"Got a ping packet"<<endl;
         t_pingPacket pingPacket(*reinterpret_cast<t_pingPacket *>(ReceiveBuffer));
         cout<<"The time difference was an amazing "<<(boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds() - pingPacket.time)<<endl<<endl;
         recieved = 1;
      }
         break;

      case 2:
      {
         cout<<"Got a connect packet"<<endl;
         t_connectPacket connectPacket(*reinterpret_cast<t_connectPacket *>(ReceiveBuffer));
         cout<<"I was given the id of "<<(connectPacket.id)<<endl<<endl;
         recieved = 1;
      }
         break;

      case 3:
      {
         cout<<"Got a world packet"<<endl;
         t_worldPacket worldPacket(*reinterpret_cast<t_worldPacket *>(ReceiveBuffer));
         cout<<"The world has "<<worldPacket.numOfObjects<<" objects"<<endl<<endl;

         std::set<uint16_t>::iterator it = objectsLeft.begin();
         
         for (int i = 0;i<worldPacket.numOfObjects;i++)
         {
            objectsLeft.insert(i);
         }

         recieved = 1;
      }
         break;
      
      case 4:
      {
         cout<<"Got a object packet"<<endl;
         t_objectPacket *objectPacket = reinterpret_cast<t_objectPacket *>(ReceiveBuffer);
         cout<<"The packet has "<<objectPacket->numOfObjects<<" objects"<<endl<<endl;

         cout<<"The first object has a name of "<<objectPacket->objectData[0].name<<endl;
         cout<<"The first object has a entName of "<<objectPacket->objectData[0].entName<<endl;
         cout<<"The first object has a sceneName of "<<objectPacket->objectData[0].sceneName<<endl;
         cout<<"The first object has a meshName of "<<objectPacket->objectData[0].meshName<<endl<<endl;
         
         cout<<"The first object has a position of "<<(Ogre::Vector3) objectPacket->objectData[0].position<<endl;
         cout<<"The first object has a orientation of "<<(Ogre::Quaternion) objectPacket->objectData[0].orientation<<endl;
         cout<<"The first object has a scale of "<<(Ogre::Vector3) objectPacket->objectData[0].scale<<endl;

         cout<<"The first object has a linearVelocity of "<<(Ogre::Vector3) objectPacket->objectData[0].linearVelocity<<endl;
         cout<<"The first object has a angularVelocity of "<<(Ogre::Vector3) objectPacket->objectData[0].angularVelocity<<endl;

         cout<<"The first object has a type of "<<objectPacket->objectData[0].type<<endl;
         cout<<"The first object has a mass of "<<objectPacket->objectData[0].mass<<endl;
         cout<<"The first object has a friction of "<<objectPacket->objectData[0].friction<<endl<<endl;

         for (int i = 0;i<objectPacket->numOfObjects;i++)
         {
            int l = objectPacket->numbers[i];
            objectsLeft.erase(l);

            mObjectData.insert(std::map<uint16_t, t_objectData>::value_type(l,objectPacket->objectData[l]));
         }

         recieved = 1;
      }
         break;

      default:
         cout<<"Who knows what packet I got!!"<<endl<<endl;
         //exit(0);
   } 
}

void Client::timeout(const boost::system::error_code &error, boost::asio::deadline_timer &timer, udp::socket *sock, void *packet,std::size_t size)
{
   if (!recieved)
   {
      sock->send(boost::asio::buffer(packet,size));

      timer.expires_from_now(boost::posix_time::milliseconds(100));
      timer.async_wait(boost::bind(&Client::timeout,this,boost::asio::placeholders::error,boost::ref(timer),sock,packet,size));
   }
}

bool Client::keyPressed(const OIS::KeyEvent &arg)
{
   cout<<"A key was pressed"<<endl;

   t_eventPacket eventPacket;

   eventPacket.eventCode = 1;
   
   if (arg.key == OIS::KC_T)
   {
      cout<<"He pressed t";
      eventPacket.eventCode = 0;
   }

   sock->send(boost::asio::buffer(&eventPacket,sizeof(eventPacket)));
   return true;
}

bool Client::keyReleased(const OIS::KeyEvent &arg)
{
   cout<<"A key was released"<<endl;
   return true;
}
