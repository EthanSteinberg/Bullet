#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <cstdint>

#include "src/main.h"

using namespace boost::asio::ip;
using std::cout;
using std::endl;

void handler(const boost::system::error_code &error, std::size_t bytes_transferred);

uint8_t ReceiveBuffer[512];

int main(int argc, char *argv[])
{

   
   if (argc != 2)
   {
      std::cout<<"Wrong Number of arguments"<<std::endl;
      exit(0);
   }

   boost::asio::io_service ioserv;

   udp::socket sock(ioserv);
   udp::resolver reso(ioserv);
   udp::resolver::query que(udp::v4(),argv[1],"");
   udp::endpoint end(*reso.resolve(que));
   end.port(1327);


   sock.connect(end);
   sock.async_receive(boost::asio::buffer(ReceiveBuffer,sizeof(ReceiveBuffer)),handler);

   t_pingPacket pingPacket;
   pingPacket.type = 1;
   pingPacket.time = boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds();

   sock.send(boost::asio::buffer(&pingPacket,sizeof(pingPacket)));
   ioserv.run();
}

void handler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
   switch(reinterpret_cast<t_Packet *>(ReceiveBuffer)->type)
   {
      case 1: 
      {
         cout<<"Got a ping packet"<<endl;
         t_pingPacket pingPacket(*reinterpret_cast<t_pingPacket *>(ReceiveBuffer));
         cout<<"The time difference was an amazing "<<(boost::posix_time::microsec_clock::universal_time().time_of_day().total_microseconds() - pingPacket.time)<<endl;
      }
         break;

      default:
         cout<<"Who knows what packet I got!!"<<endl;
         exit(0);
   }  
}
               
