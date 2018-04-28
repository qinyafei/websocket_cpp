cmake_minimum_required(VERSION 2.8)

project(websocket CXX)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "debug")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
add_definitions(-g)



include_directories(.)
include_directories(base)
include_directories(log)
include_directories(net)
include_directories(websocket)

LINK_DIRECTORIES(lib)


aux_source_directory(log GLOG_SRC)
aux_source_directory(websocket WS_SRC)
aux_source_directory(base BASE_SRC)
aux_source_directory(net NET_SRC)

SET(SRC_LIST 
${GLOG_SRC} 
${WS_SRC} 
${BASE_SRC} 
${NET_SRC} 
)

SET (LIBS libglog.a pthread)

SET(PROJECT_ROOT_PATH ${CMAKE_SOURCE_DIR})                 #工程的根目录，即test
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")    #可执行生成后存放的目录(CMAKE_SOURCE_DIR是cmake目录)
#SET(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/build")             #静态库生成后存放的目录

add_executable(ws_client ${SRC_LIST} ws_client.cpp)
TARGET_LINK_LIBRARIES(ws_client ${LIBS})


add_executable(ws_server ${SRC_LIST} ws_server.cpp)
TARGET_LINK_LIBRARIES(ws_server ${LIBS})

