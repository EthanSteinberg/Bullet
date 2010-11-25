#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <cstdint>

#include <map>
#include <set>

#include "../network/network.h"

bool recieved = 0;

using namespace boost::asio::ip;

using std::cout;
using std::endl;

void handler(const boost::system::error_code &error, std::size_t bytes_transferred);
void pingtimeout(const boost::system::error_code &error,boost::asio::deadline_timer &timer, udp::socket &sock,t_pingPacket &pingPacket);
void connecttimeout(const boost::system::error_code &error,boost::asio::deadline_timer &timer, udp::socket &sock,t_connectPacket &connectPacket);

template<class t_packetType>
void timeout(const boost::system::error_code &error, boost::asio::deadline_timer &timer, udp::socket &sock, t_packetType packetType)
{
   if (!recieved)
   {
      sock.send(boost::asio::buffer(&packetType,sizeof(packetType)));

      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(timeout<t_packetType> ,boost::asio::placeholders::error,boost::ref(timer),boost::ref(sock),boost::ref(packetType)));
   }
}

uint8_t ReceiveBuffer[512];

std::set<uint16_t> objectsLeft;

int main(int argc, char *argv[])
{

   btVector3 test(3,2,1);
   myVector3 sos = test;
   cout<<(Ogre::Vector3)sos<<endl;

   if (argc != 2)
   {
      std::cout<<"Wrong Number of arguments"<<std::endl;
      exit(0);
   }

   boost::asio::io_service ioserv;
   boost::asio::deadline_timer timer(ioserv);

   udp::socket sock(ioserv);
   udp::resolver reso(ioserv);
   udp::resolver::query que(udp::v4(),argv[1],"");
   udp::endpoint end(*reso.resolve(que));
   end.port(1327);
   
   sock.connect(end);
   
   {
      sock.async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(handler,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
   
      recieved = 0;

      t_pingPacket pingPacket;
      pingPacket.time = boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds();

      sock.send(boost::asio::buffer(&pingPacket,sizeof(pingPacket)));
   
      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(timeout<t_pingPacket>,boost::asio::placeholders::error,boost::ref(timer),boost::ref(sock), boost::ref(pingPacket)));
   
      ioserv.run();
      ioserv.reset();
   }

   {
      sock.async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(handler,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
   
      recieved = 0;
   
      t_connectPacket connectPacket;

      sock.send(boost::asio::buffer(&connectPacket,sizeof(connectPacket)));

      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(timeout<t_connectPacket>,boost::asio::placeholders::error,boost::ref(timer),boost::ref(sock), boost::ref(connectPacket)));
   
      ioserv.run();
      ioserv.reset();
   }
   
   {
      sock.async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(handler,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
   
      recieved = 0;
   
      t_worldPacket worldPacket;

      sock.send(boost::asio::buffer(&worldPacket,sizeof(worldPacket)));

      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(timeout<t_worldPacket>,boost::asio::placeholders::error,boost::ref(timer),boost::ref(sock), boost::ref(worldPacket)));
   
      ioserv.run();
      ioserv.reset();
   }

   while (!objectsLeft.empty())
   {
      sock.async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),boost::bind(handler,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
      
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
      
      sock.send(boost::asio::buffer(&objectPacket,sizeof(objectPacket)));
      
      timer.expires_from_now(boost::posix_time::seconds(1));
      timer.async_wait(boost::bind(timeout<t_objectPacket>,boost::asio::placeholders::error,boost::ref(timer),boost::ref(sock), boost::ref(objectPacket)));
      
      ioserv.run();
      ioserv.reset();
   }
}

void handler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
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

         cout<<"The first object has a name of "<<objectPacket->objectData[0].name<<endl<<endl;
         cout<<"The first object has a name of "<<objectPacket->objectData[0].entName<<endl<<endl;
         cout<<"The first object has a name of "<<objectPacket->objectData[0].sceneName<<endl<<endl;
         cout<<"The first object has a name of "<<objectPacket->objectData[0].meshName<<endl<<endl;
         
         cout<<"The first object has a name of "<<(Ogre::Vector3) objectPacket->objectData[0].position<<endl<<endl;
         cout<<"The first object has a name of "<<objectPacket->objectData[0].orientation<<endl<<endl;
         cout<<"The first object has a name of "<<(Ogre::Vector3) objectPacket->objectData[0].linearVelocity<<endl<<endl;
         cout<<"The first object has a name of "<<(Ogre::Vector3) objectPacket->objectData[0].angularVelocity<<endl<<endl;

         for (int i = 0;i<objectPacket->numOfObjects;i++)
         {
            objectsLeft.erase(objectPacket->numbers[i]);
         }

         recieved = 1;
      }
         break;

      default:
         cout<<"Who knows what packet I got!!"<<endl<<endl;
         //exit(0);
   } 
}
