#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/bimap.hpp>

#include <iostream>
#include <cstdint>
#include <vector>

#include "../network/network.h"
#include "server.h"

using std::cout;
using std::endl;

using namespace boost::asio::ip;

void Server::start()
{
   CurId = 0;

   udp::endpoint ends(udp::v4(),1327);
   
   sock = new udp::socket(ioserv,ends);
   end = new udp::endpoint;
   
   sock->async_receive_from(boost::asio::buffer(ReceiveBuffer),*end,boost::bind(&Server::serverHandler,this,_1,_2));
}

void Server::run()
{
   ioserv.run();
}

void Server::serverHandler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
   switch(reinterpret_cast<t_Packet *>(ReceiveBuffer)->type)
   {
      case 0:
      {
         cout<<"I got a default packet"<<endl;
      }
         break;

      case 1: 
      {
         cout<<"Got a ping packet"<<endl;
         cout<<"Its from "<<end->address().to_string()<<endl; 
         
         t_pingPacket pingPacket(*reinterpret_cast<t_pingPacket *>(ReceiveBuffer));
         
         cout<<"The time difference was an amazing "<<(boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds() - pingPacket.time)<<endl;
         cout<<"Sending it back...\n\n"<<endl;
         sock->send_to(boost::asio::buffer(ReceiveBuffer),*end);
      }
         break;
      
      case 2:
      {
         cout<<"Someone from "<<end->address().to_string()<<" is trying to join"<<endl;
         
         t_connectPacket connectPacket;
         
         bm::right_map::const_iterator lol = table.right.find(*end);
         if (table.right.end() == lol)
         {
            cout<<"They are not already connected"<<endl;

            connectPacket.id = CurId;
            table.insert(bm::value_type(CurId++,*end));
         }

         else
         {
            cout<<"They connected before"<<endl;

            connectPacket.id=lol->second;
         }

         sock->send_to(boost::asio::buffer(&connectPacket,sizeof(connectPacket)),*end);
         cout<<"We gave them an id of "<<(connectPacket.id)<<endl<<endl;
      }
         break;
      
      case 3:
      {
         cout<<"Someone from "<<end->address().to_string()<<" with id "<<table.right.find(*end)->second<<" wants the world"<<endl;

         t_worldPacket worldPacket(*reinterpret_cast<t_worldPacket *>(ReceiveBuffer));
         worldPacket.numOfObjects = mCopyData.size();

         cout<<"Telling them the world has "<<worldPacket.numOfObjects<<" objects"<<endl<<endl;
         sock->send_to(boost::asio::buffer(&worldPacket,sizeof(worldPacket)),*end);
      }
         break;

      case 4:
      {
         cout<<"Someone from "<<end->address().to_string()<<" with id "<<table.right.find(*end)->second<<" wants some objects"<<endl;

         t_objectPacket objectPacket(*reinterpret_cast<t_objectPacket *>(ReceiveBuffer));
         
         for (int i = 0;i<objectPacket.numOfObjects;i++)
         {
            cout<<"He wants the object "<<objectPacket.numbers[i]<<endl;
         }
         
         cout<<"\nMaking new packet"<<endl;

         t_objectPacket *newObjectPacket = reinterpret_cast<t_objectPacket *>(malloc(sizeof(t_objectPacket) + objectPacket.numOfObjects * sizeof(t_objectData)));
         memcpy(newObjectPacket,&objectPacket,sizeof(objectPacket));
      
         cout<<"Created new packet of right size(hopefully)"<<endl;
         cout<<"Setting them all to zero, so I can be lazy"<<endl; 

         for (int i = 0;i<objectPacket.numOfObjects;i++)
         {
            int l = objectPacket.numbers[i];
            t_CopyData temp = mCopyData[l];

            strcpy(newObjectPacket->objectData[l].name,temp.name.c_str());
            strcpy(newObjectPacket->objectData[l].sceneName,temp.sceneName.c_str());
            strcpy(newObjectPacket->objectData[l].entName,temp.entName.c_str());
            strcpy(newObjectPacket->objectData[l].meshName,temp.meshName.c_str());

            Ogre::SceneNode *node = mStore[temp.name].node; 
            btRigidBody *body = mStore[temp.name].body;

            newObjectPacket->objectData[l].position = node->getPosition(); 
            newObjectPacket->objectData[l].orientation = node->getOrientation(); 
            
            newObjectPacket->objectData[l].linearVelocity = body->getLinearVelocity();
            newObjectPacket->objectData[l].angularVelocity = body->getAngularVelocity();
         }
         
         cout<<"Finially sending the stupid thing over, it has a size of "<<(sizeof(t_objectPacket) + objectPacket.numOfObjects * sizeof(t_objectData))<<endl<<endl;
         sock->send_to(boost::asio::buffer(newObjectPacket,sizeof(t_objectPacket) + objectPacket.numOfObjects * sizeof(t_objectData)),*end);
      }

      default:
         cout<<"Who knows what packet I got!!"<<endl;
   }  
   
   sock->async_receive_from(boost::asio::buffer(ReceiveBuffer),*end,boost::bind(&Server::serverHandler,this,_1,_2));
}
