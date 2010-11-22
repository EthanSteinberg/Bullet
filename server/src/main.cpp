#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <cstdint>
#include <src/main.h>

using namespace boost::asio::ip;

using std::cout;
using std::endl;

uint8_t ReceiveBuffer[512];

void handler(const boost::system::error_code &error, std::size_t bytes_transferred, udp::socket &sock, udp::endpoint *end);

int main()
{
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

      default:
         cout<<"Who knows what packet I got!!"<<endl;
   }  
   
   sock.async_receive_from(boost::asio::buffer(ReceiveBuffer),*end,boost::bind(handler,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,
      boost::ref(sock),
      end));
}
