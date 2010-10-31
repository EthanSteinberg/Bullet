#include <OGRE/Ogre.h>

class Test
{
public:
   Test();
   ~Test();
   bool go();

private:
   Ogre::Root *mRoot;
   Ogre::String mPluginsCfg;
   Ogre::String mResourcesCfg;
};

