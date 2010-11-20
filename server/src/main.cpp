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

void handler(const boost::system::error_code &error, std::size_t bytes_transferred, udp::socket &sock, const udp::endpoint &end);

int main()
{
   boost::asio::io_service ioserv;
   udp::endpoint ends(udp::v4(),1327);
   udp::socket sock(ioserv,ends);


   udp::endpoint end;
   
   sock.async_receive_from(boost::asio::buffer(ReceiveBuffer),end,boost::bind(handler,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,
      boost::ref(sock),
      boost::cref(end)));

   sock.async_receive_from(boost::asio::buffer(ReceiveBuffer),end,boost::bind(handler,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,
      boost::ref(sock),
      boost::cref(end)));
   
   ioserv.run();
}

void handler(const boost::system::error_code& error, std::size_t /*bytes_transferred*/, udp::socket &sock, const udp::endpoint &end)
{
   switch(reinterpret_cast<t_Packet *>(ReceiveBuffer)->type)
   {
      case 1: 
         cout<<"Got a ping packet, so sending it back"<<endl;
         cout<<"Its from "<<end.address().to_string()<<endl; 
         sock.send_to(boost::asio::buffer(ReceiveBuffer),end);
         break;

      default:
         cout<<"Who knows what packet I got!!"<<endl;
   }  
}
