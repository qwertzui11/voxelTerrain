cmake_minimum_required (VERSION 2.8)
project (voxelTerrain)

# The version number.
set (blub_VERSION_MAJOR 1)
set (blub_VERSION_MINOR 2)

# Initialize CXXFLAGS.
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS                "-Wall -std=c++11 -Wno-unused-variable ${CMAKE_CXX_FLAGS}")
endif()
#set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -D_DEBUG")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_DEBUG_POSTFIX 	   "_d")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
# for ccache
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
elseif (MSVC AND NOT (MSVC_VERSION LESS 1700)) # minimum Visual Studio 2012 (msvc11)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4305 /wd4244 /EHsc")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11. CMAKE_CXX_COMPILER_ID:${CMAKE_CXX_COMPILER_ID} ${MSVC_VERSION}")
endif ()


# options
if (NOT MSVC)
  option (BUILD_SHARED_LIBS "build as shared libraries" ON)
else()
  option (BUILD_SHARED_LIBS "build as shared libraries" OFF)
  if (BUILD_SHARED_LIBS)
	message( FATAL_ERROR "Only static-library-building is supported, because libblub misses Microsofts __declspec(...). - We'll fix." )
  endif()
endif(NOT MSVC)

option (BLUB_BUILD_ASYNC "build async" ON)
#option (BLUB_BUILD_DATABASE "build database" ON)
#option (BLUB_BUILD_GRAPHIC "build graphic" ON)
#option (BLUB_BUILD_GUI "build gui" ON)
#option (BLUB_BUILD_INPUT "build input" ON)
option (BLUB_BUILD_MATH "build math" ON)
#option (BLUB_BUILD_NETWORK "build network" ON)
#option (BLUB_BUILD_PHYSIC "build physic" ON)
option (BLUB_BUILD_PROCEDURAL "build graphic" ON)
option (BLUB_BUILD_SERIALIZATION "build serialization" ON)
#option (BLUB_BUILD_SIGNALS "build signals" ON)
option (BLUB_BUILD_SYNC "build sync" ON)
#option (BLUB_BUILD_WEB "build web" ON)

option (BLUB_BUILD_EXAMPLES "build examples" ON)
#option (BLUB_BUILD_TESTS "build tests" ON)

option (BLUB_USE_ASSIMP "use assimp" ON)
#option (BLUB_USE_BULLET "use bullet" ON)
#option (BLUB_USE_PHYSX "use physx" OFF)
#option (BLUB_USE_CEF3 "use cef3" ON)
option (BLUB_USE_OGRE3D "use ogre3d" ON)
option (BLUB_USE_OIS "use ois" ON)
#option (BLUB_USE_SOCI "use soci" ON)

unset (LIBS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


# find boost
ADD_DEFINITIONS( -DBOOST_ALL_DYN_LINK )
if (NOT MSVC)
  set( BOOST_ROOT "/usr/local" CACHE PATH "boost root")
else()
  set( BOOST_ROOT "C:/local" CACHE PATH "boost root")
endif()
IF(NOT EXISTS ${BOOST_ROOT})
  message(FATAL_ERROR "Could not find boost root: ${BOOST_ROOT}")
endif ()
if (MSVC)
  find_package(Boost 1.55.0 REQUIRED COMPONENTS log log_setup system thread chrono date_time iostreams serialization unit_test_framework filesystem bzip2  )
else()
  find_package(Boost 1.55.0 REQUIRED COMPONENTS log log_setup system thread chrono date_time iostreams serialization unit_test_framework )
endif()
set (INCLUDES ${INCLUDES}
  ${Boost_INCLUDE_DIRS}
)
set (LIBS ${LIBS}
  ${Boost_LIBRARIES}
)

# find bzip2
if (NOT MSVC)
  find_package(BZip2 REQUIRED)
  set (LIBS ${LIBS}
    ${BZIP2_LIBRARIES}
  )
endif()

# ogre3d
if(BLUB_USE_OGRE3D)
  find_package(OGRE REQUIRED)
  set (INCLUDES ${INCLUDES}
    ${OGRE_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${OGRE_LIBRARIES}
  )
endif(BLUB_USE_OGRE3D)

# ois
if(BLUB_USE_OIS)
  find_package(OIS REQUIRED)
  set (INCLUDES ${INCLUDES}
       ${OIS_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${OIS_LIBRARIES}
  )
endif(BLUB_USE_OIS)

# assimp
if(BLUB_USE_ASSIMP)
  find_package(Assimp REQUIRED)
  set (INCLUDES ${INCLUDES}
    ${ASSIMP_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${ASSIMP_LIBRARIES}
  )
endif(BLUB_USE_ASSIMP)


if (BLUB_BUILD_DATABASE)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} database)
endif (BLUB_BUILD_DATABASE)
if (BLUB_BUILD_PHYSIC)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} physic)
endif (BLUB_BUILD_PHYSIC)
if (BLUB_BUILD_GRAPHIC)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} graphic)
endif (BLUB_BUILD_GRAPHIC)
if (BLUB_BUILD_INPUT)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} input)
endif (BLUB_BUILD_INPUT)
if (BLUB_BUILD_ASYNC)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} async)
endif (BLUB_BUILD_ASYNC)
if (BLUB_BUILD_GUI)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} gui)
endif (BLUB_BUILD_GUI)
if (BLUB_BUILD_MATH)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} math)
endif (BLUB_BUILD_MATH)
if (BLUB_BUILD_NETWORK)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} network)
endif (BLUB_BUILD_NETWORK)
if (BLUB_BUILD_PROCEDURAL)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} procedural)
endif (BLUB_BUILD_PROCEDURAL)
#if (BLUB_BUILD_SERIALIZATION)
#  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} serialization)
#endif (BLUB_BUILD_SERIALIZATION)
if (BLUB_BUILD_SIGNALS)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} signals)
endif (BLUB_BUILD_SIGNALS)
if (BLUB_BUILD_SYNC)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} sync)
endif (BLUB_BUILD_SYNC)
if (BLUB_BUILD_WEB)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} web)
endif (BLUB_BUILD_WEB)

set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD}
 core
 log
 serialization
)

# create config files
# globals.hpp
# config includes created at the documents end
set (INCLUDES ${INCLUDES}
  "${PROJECT_BINARY_DIR}/config_files/"
  "${PROJECT_BINARY_DIR}/config_files/blub/core"
)
configure_file (
  "${PROJECT_SOURCE_DIR}/modules/core/source/blub/core/globals.hpp.in"
  "${PROJECT_BINARY_DIR}/config_files/blub/core/globals.hpp"
  )
# for the "export.hpp"s
include (GenerateExportHeader)
#ADD_COMPILER_EXPORT_FLAGS()

# a macro to prefix list of strings
macro (prefix_list prefix list result)
  foreach(to_prefix ${${list}})
    set(${result} ${${result}} "${prefix}${to_prefix}")
  endforeach(to_prefix)
endmacro (prefix_list)

# add all includes - every module shall know about each header
foreach(LIB_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD})
  set (INCLUDES ${INCLUDES}
    ${PROJECT_SOURCE_DIR}/modules/${LIB_TO_BUILD}/source
  )
endforeach(LIB_TO_BUILD)

# add modules
add_subdirectory(modules)

# do examples
if (BLUB_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BLUB_BUILD_EXAMPLES)

# do tests
if (BLUB_BUILD_TESTS)
  add_subdirectory(tests)
endif(BLUB_BUILD_TESTS)

install (DIRECTORY "${PROJECT_BINARY_DIR}/config_files/"
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")

