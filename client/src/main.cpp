#include <iostream>
#include "client.h"

using std::cout;
using std::endl;

int main()
{
   Client test;

   try{ 
   test.go(); 
   cout<<"Finished Running";}

   catch(Ogre::Exception& e)
   {
      cout<<"An exception has occured: "<<e.getFullDescription().c_str()<<endl;
   }
   
   catch(std::exception &e)
   {
      cout<<"An exception has occured: "<<e.what()<<endl;
   }

   catch(...)
   {
      cout<<"Something bad happened"<<endl;
      exit(0);
   }

}
