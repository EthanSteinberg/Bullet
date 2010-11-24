
#CMakeLists file for client

cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++0x")

set(HDRS 
      src/main.h
      src/BulletXML.h
      #utils/DotSceneLoader.h
      #utils/rapidxml.hpp
       
      #utils/BtOgreExtras.h
      #utils/BtOgrePG.h
      #utils/BtOgreGP.h
      )
 
set(SRCS
      src/main.cpp
      #utils/DotSceneLoader.cpp
      #utils/BtOgre.cpp
)

add_library(BulletXML src/BulletXML.h ../utils/rapidxml.hpp src/BulletXML.cpp)
 
add_executable(OgreApp ${HDRS} ${SRCS})
 
set_target_properties(OgreApp PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(OgreApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${BULLET_LIBRARIES} BtOgre DotSceneLoader BulletXML)
