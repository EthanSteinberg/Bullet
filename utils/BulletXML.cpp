#include "BulletXML.h"
#include "../utils/rapidxml.hpp"
#include "../utils/BtOgreGP.h"
#include "../utils/BtOgrePG.h"

#include "server/src/server.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>

#include <cstring>
#include <cassert>

using namespace rapidxml;

BulletXML::BulletXML(const char *name, btDiscreteDynamicsWorld *World,Ogre::SceneManager *SceneMgr, std::map<uint16_t, t_CopyData> *CopyData,std::map<std::string, std::string> *Meshes, std::map<std::string, t_Store> *Store, Server *server) 
   : file(name), mWorld(World), mSceneMgr(SceneMgr), mCopyData(CopyData), mMeshes(Meshes) ,mStore(Store), objectNum(0), mServer(server)
{
}

void BulletXML::parse()
{
   char *test = loadFile(file);
   std::cout<<test;
      
   xml_document<> doc;
   doc.parse<0>(test);

   xml_node<> *RootNode = doc.first_node();

   if (strcmp(RootNode->name(),"bullets"))
   {   
      std::cout<<"Not the right file";
      std::cout<<"It starts with"<<doc.first_node()->name()<<std::endl;
      exit(0);
   }

   xml_node<> *ObjectRoot = RootNode->first_node("objects");
   assert(ObjectRoot);

   parseObjects(ObjectRoot);
}

char *BulletXML::loadFile(const char *name)
{
   std::ifstream myFile(name);

   myFile.seekg(0,std::ios::end);
   int length = myFile.tellg();
   myFile.seekg(0,std::ios::beg);
   
   char *test = new char [length + 1];
   myFile.read(test,length);
   test[length] = 0;
   myFile.close();

   return test;
}

void BulletXML::parseObjects(xml_node<> *ObjectRoot)
{
   xml_node<> *ObjectNode = ObjectRoot->first_node();
   do
   {
      parseObject(ObjectNode);
      loadObject();
   }
   while ((ObjectNode = ObjectNode->next_sibling()));
}

void BulletXML::parseObject(xml_node<> *ObjectNode)
{
   BulletObject.name = ObjectNode->first_attribute("Name")->value();
   BulletObject.entName = ObjectNode->first_attribute("EntName")->value();
   BulletObject.sceneName = ObjectNode->first_attribute("SceneName")->value();
   BulletObject.friction = boost::lexical_cast<double>(ObjectNode->first_attribute("friction")->value());
   BulletObject.mass = boost::lexical_cast<double>(ObjectNode->first_attribute("mass")->value());
   BulletObject.type = boost::lexical_cast<int>(ObjectNode->first_attribute("type")->value());

   std::cout<<"The name is "<<BulletObject.name<<std::endl;
   std::cout<<"The entName is "<<BulletObject.entName<<std::endl;
   std::cout<<"The sceneName is "<<BulletObject.sceneName<<std::endl;
   std::cout<<"The friction is "<<BulletObject.friction<<std::endl;
   std::cout<<"The mass is "<<BulletObject.mass<<std::endl;
   std::cout<<"The type is "<<BulletObject.type<<std::endl<<std::endl;

   t_CopyData temp;
   temp.name = BulletObject.name;
   temp.entName = BulletObject.entName;
   temp.sceneName = BulletObject.sceneName;
   temp.friction = BulletObject.friction;
   temp.mass = BulletObject.mass;
   temp.type = BulletObject.type;

   temp.meshName = (*mMeshes)[BulletObject.entName];

   (*mCopyData)[objectNum++] = temp;
}

void BulletXML::loadObject()
{
   t_Store temp;

   temp.entity = mSceneMgr->getEntity(BulletObject.entName);
   temp.node = mSceneMgr->getSceneNode(BulletObject.sceneName);
   
   //Create shape.
   BtOgre::StaticMeshToShapeConverter converter(temp.entity);
   switch (BulletObject.type)
   {
      case 0:
         temp.shape = converter.createBox();
         break;

      case 1:
         temp.shape = converter.createTrimesh();
         break;
   }

   //Calculate inertia.
   btScalar mass = BulletObject.mass;
   btVector3 inertia;
   temp.shape->calculateLocalInertia(mass, inertia);

   //Create BtOgre MotionState (connects Ogre and Bullet).
   auto *mState = new myMotionState(temp.node,BulletObject.name,mServer);

   //Create the Body.
   btRigidBody::btRigidBodyConstructionInfo mRigidInfo(BulletObject.mass, mState, temp.shape, inertia);
   mRigidInfo.m_friction = BulletObject.friction;

   temp.body = new btRigidBody(mRigidInfo);
   mWorld->addRigidBody(temp.body);

   (*mStore)[BulletObject.name] = temp;
}
