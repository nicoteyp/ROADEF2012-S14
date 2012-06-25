cmake_minimum_required(VERSION 2.6)
#===============================================================================    
# PROJECT DEFINITION
#===============================================================================

set(REALLOC "REALLOC")
set(ProjectName "${REALLOC}")
project(${ProjectName})
option(REALLOC_OPENMP "parallel program" OFF)


#===============================================================================    
# VERSION
#===============================================================================
set(CPACK_PACKAGE_VERSION_MAJOR 2)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_VENDOR "Probayes SAS")


#===============================================================================
#LEMON INCLUDE PATH DEFINITION
#===============================================================================

#DEFINE YOUR LEMON PATH
set(LEMON_INCLUDE_PATH "./lemon-1.2.3/")


#===============================================================================    
# MAIN PROGRAM
#===============================================================================
set(CMAKE_CXX_FLAGS_DEBUG  "-ggdb3 -DDEBUG -fPIC -O -W -Wno-unused -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc")
set(EXECSTATIC "machineReassignment")
set(EXECSTATIC_INCLUDE_PATH "include")
file(GLOB EXECSTATIC_SRC "src/*.cpp")

include_directories(${LEMON_INCLUDE_PATH})
include_directories(${EXECSTATIC_INCLUDE_PATH})
add_executable(${EXECSTATIC} ${EXECSTATIC_SRC})

