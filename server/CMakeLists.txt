#CMakeLists file for server

cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wshadow")
set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++0x")

set(HDRS 
      src/network.h
      )
 
set(SRCS
      src/main.cpp
      src/serverGraphics.cpp
      src/serverNetwork.cpp
)
 
add_executable(OgreSer ${HDRS} ${SRCS}) 
set_target_properties(OgreSer PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(OgreSer ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${BULLET_LIBRARIES} BulletXML BtOgre DotSceneLoader)




add_executable(OgreCli src/network.h src/client.cpp)
set_target_properties(OgreCli PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(OgreCli ${BULLET_LIBRARIES} ${OGRE_LIBRARIES} ${Boost_LIBRARIES} stdc++)
