#ifndef BULLETXML_H_INCLUDED
#define BULLETXML_H_INCLUDED

#include "../utils/rapidxml.hpp"
#include <btBulletDynamicsCommon.h>
#include <OGRE/Ogre.h>
#include <string>
#include <map>
#include <cstdint>

struct t_CopyData
{
   std::string name;
   std::string entName;
   std::string sceneName;
   std::string meshName;

   double friction;
   double mass;

   int type;
};

struct t_Store
{
   btRigidBody *body;
   btCollisionShape *shape;

   Ogre::SceneNode *node;
   Ogre::Entity *entity;
};

class BulletXML
{
public:
   BulletXML(const char *name, btDiscreteDynamicsWorld *World,Ogre::SceneManager *SceneMgr, std::map<uint16_t, t_CopyData> *CopyData,std::map<std::string, std::string> *Meshes, std::map<std::string, t_Store> *Store);
   void parse();

private:
   char *loadFile(const char *name);
   void parseObjects(rapidxml::xml_node<> *ObjectRoot);
   void parseObject(rapidxml::xml_node<> *ObjectNode);
   void loadObject();

   const char *file;
   btDiscreteDynamicsWorld *mWorld;
   Ogre::SceneManager *mSceneMgr;

   struct
   {
      char *name;
      char *entName;
      char *sceneName;
   
      double friction;
      double mass;

      int type;
   } BulletObject;

   std::map<uint16_t, t_CopyData> *mCopyData;
   std::map<std::string, std::string> *mMeshes;
   std::map<std::string, t_Store> *mStore;
   
   uint16_t objectNum; 
};

#endif
