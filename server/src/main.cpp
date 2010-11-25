#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/bimap.hpp>

#include <iostream>
#include <cstdint>
#include <vector>

#include "server.h"

using namespace boost::asio::ip;

using std::cout;
using std::endl;

int main()
{
   try
   {
      Server test;
      test.start();
      test.go();

      test.run();
   }

   catch(Ogre::Exception& e)
   {
      cout<<"An exeption has occured: "<<e.getFullDescription().c_str()<<endl;
   }

   catch(std::exception &e)
   {
      cout<<e.what()<<endl;
   }

   catch(...)
   {
      cout<<"Some thing bad happened"<<endl;
   }
}

