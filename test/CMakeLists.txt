cmake_minimum_required(VERSION 3.14.0)

project(testepoll)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
#  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

#CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

#find_package(libev REQUIRED)
include_directories(/usr/local/include /usr/local/include/coepoll )

aux_source_directory(./ SRC_DIR)

set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
set(CMAKE_CXX_FLAGS "-ggdb -O3 -std=c++14")

link_directories(/usr/local/lib /usr/local/lib64)
add_executable(${PROJECT_NAME} ${SRC_DIR})
#add_library(${PROJECT_NAME} STATIC ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} coepoll pthread ev boost_context dl)
