#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/bimap.hpp>

#include <iostream>
#include <cstdint>
#include <vector>

#include "src/main.h"

using namespace boost::asio::ip;


using std::cout;
using std::endl;

uint8_t ReceiveBuffer[512];
int CurId = 0;

//std::vector<udp::endpoint> table;

typedef boost::bimap<boost::bimaps::set_of<int>, boost::bimaps::set_of<udp::endpoint> > bm;

bm table;

void handler(const boost::system::error_code &error, std::size_t bytes_transferred, udp::socket &sock, udp::endpoint *end);

int main()
{
   cout<<"The size of an object is "<<sizeof(t_objectData)<<" and "<<sizeof(t_objectPacket)<<endl;

   boost::asio::io_service ioserv;
   udp::endpoint ends(udp::v4(),1327);
   udp::socket sock(ioserv,ends);


   udp::endpoint *end = new udp::endpoint;
   
   sock.async_receive_from(boost::asio::buffer(ReceiveBuffer),*end,boost::bind(handler,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,
      boost::ref(sock),
      end));

   ioserv.run();
}

void handler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/, udp::socket &sock, udp::endpoint *end)
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
         sock.send_to(boost::asio::buffer(ReceiveBuffer),*end);
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

         sock.send_to(boost::asio::buffer(&connectPacket,sizeof(connectPacket)),*end);
         cout<<"We gave them an id of "<<(connectPacket.id)<<endl<<endl;
      }
         break;
      
      case 3:
      {
         cout<<"Someone from "<<end->address().to_string()<<" with id "<<table.right.find(*end)->second<<" wants the world"<<endl;

         t_worldPacket worldPacket(*reinterpret_cast<t_worldPacket *>(ReceiveBuffer));
         worldPacket.numOfObjects = 6;

         cout<<"Telling them the world has five objects"<<endl<<endl;
         sock.send_to(boost::asio::buffer(&worldPacket,sizeof(worldPacket)),*end);
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

         //memset(newObjectPacket->objectData,0,newObjectPacket->numOfObjects * sizeof(t_objectData));

         cout<<"Setting first to name ";
         strcpy(newObjectPacket->objectData[0].name,"wow");
         cout<<newObjectPacket->objectData[0].name<<endl;

         cout<<"Finially sending the stupid thing over, it has a size of "<<(sizeof(t_objectPacket) + objectPacket.numOfObjects * sizeof(t_objectData))<<endl<<endl;
         sock.send_to(boost::asio::buffer(newObjectPacket,sizeof(t_objectPacket) + objectPacket.numOfObjects * sizeof(t_objectData)),*end);


      }

      default:
         cout<<"Who knows what packet I got!!"<<endl;
   }  
   
   sock.async_receive_from(boost::asio::buffer(ReceiveBuffer),*end,boost::bind(handler,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,
      boost::ref(sock),
      end));
}
