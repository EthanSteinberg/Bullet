#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/bimap.hpp>

#include <iostream>
#include <cstdint>
#include <vector>

#include "src/network.h"

using namespace boost::asio::ip;

using std::cout;
using std::endl;

int main()
{
   cout<<"The size of an object is "<<sizeof(t_objectData)<<" and "<<sizeof(t_objectPacket)<<endl;

   server test;
   test.start();
}

