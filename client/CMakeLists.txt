
#CMakeLists file for client

cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++0x")

set(HDRS 
      src/main.h
      
      #utils/BulletXML.h
      #utils/DotSceneLoader.h
      #utils/BtOgreExtras.h
      #utils/BtOgrePG.h
      #utils/BtOgreGP.h
      )
 
set(SRCS
      src/main.cpp
)

add_executable(OgreApp ${HDRS} ${SRCS})
 
set_target_properties(OgreApp PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(OgreApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${BULLET_LIBRARIES} BtOgre DotSceneLoader BulletXML)
