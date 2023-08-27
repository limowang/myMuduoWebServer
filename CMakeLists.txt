cmake_minimum_required(VERSION 2.6)

project(myMuduoWebServer C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

include_directories(
  BaseModule 
  HttpModule 
  LoggingModule 
  TimerModule 
  Util
)

SET(CXX_FLAGS
  -g
  -Wall
  -Wextra
  -Werror
  -Wconversion
  -Wshadow
  -std=c++11
)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Threads REQUIRED)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})
message(STATUS "CMAKE_THREAD_LIBS_INIT = " ${CMAKE_THREAD_LIBS_INIT})

aux_source_directory(BaseModule SRC_LIST1)
aux_source_directory(HttpModule SRC_LIST2)
aux_source_directory(LoggingModule SRC_LIST3)
aux_source_directory(TimerModule SRC_LIST4)
aux_source_directory(Util SRC_LIST5)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(myMuduoWebServer main.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})

target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
