#include "main.h"
#include <OGRE/Ogre.h>

#include <iostream>

using namespace std;

Test::Test() : mRoot(0), mPluginsCfg(Ogre::StringUtil::BLANK)
{
}

Test::~Test()
{
   delete mRoot;
}

bool Test::go()
{
   #ifdef _DEBUG
   mResourcesCfg = "resources_d.cfg";
   mPluginsCfg = "plugins_d.cfg";
   #else
   mResourcesCfg = "resources.cfg";
   mPluginsCfg = "plugins.cfg";
   #endif

   Ogre::ConfigFile cf;
   cf.load(mResourcesCfg);

   Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    
   Ogre::String secName, typeName, archName;
   while (seci.hasMoreElements())
   {
      secName = seci.peekNextKey();
      Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
      Ogre::ConfigFile::SettingsMultiMap::iterator i;
      for (i = settings->begin(); i != settings->end(); ++i)    
      {
	 typeName = i->first;
	 archName = i->second;
	 Ogre::ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
      }
   }

   mRoot = new Ogre::Root(mPluginsCfg);

   return true;
}

int main(int argc,char  *argv[])
{
   Test test;

   try{ test.go(); }
   catch(Ogre::Exception& e)
   {
      cout<<"An exeption has occured: "<<e.getFullDescription().c_str()<<endl;
   }

}
