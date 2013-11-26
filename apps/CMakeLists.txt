cmake_minimum_required (VERSION 2.8)
project (libblub)

# The version number.
set (blub_VERSION_MAJOR 1)
set (blub_VERSION_MINOR 0)

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11 -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_DEBUG_POSTFIX 	   "_d")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# for ccache
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
#glib-2 throws depricated 'register'
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()


# options
option (BUILD_SHARED_LIBS "build as shared libraries" ON)

option (BLUB_BUILD_ASYNC "build async" ON)
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
option (BLUB_USE_OGRE3D "use ogre3d" ON)
option (BLUB_USE_OIS "use ois" ON)
#option (BLUB_USE_BULLET "use bullet" ON)
#option (BLUB_USE_CEF3 "use cef3" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(BLUB_LIBRARIES_TO_BUILD 
 core
)

set(EXAMPLE_EXECUTEABLES_TO_BUILD ${EXAMPLE_EXECUTEABLES_TO_BUILD}
  voxelTerrain
)

# find boost
set( BOOST_ROOT "/usr/local" CACHE PATH "boost root")
IF(NOT EXISTS ${BOOST_ROOT})
  message(FATAL_ERROR "Could not find boost root: ${BOOST_ROOT}")
endif ()
find_package(Boost REQUIRED COMPONENTS system thread chrono date_time iostreams serialization )
set (INCLUDES ${INCLUDES}
  ${Boost_INCLUDE_DIRS}
)
set (LIBS ${LIBS}
  ${Boost_LIBRARIES}
)

# find bzip2
find_package(BZip2 REQUIRED)
set (LIBS ${LIBS}
  ${BZIP2_LIBRARIES}
)

# physics (bullet)
if(BLUB_USE_BULLET)
  set( BULLET_ROOT "/usr/local" CACHE PATH "bullet root")
  IF(NOT EXISTS ${BULLET_ROOT})
    message(FATAL_ERROR "Could not find bullet root: ${BULLET_ROOT}")
  endif ()
  find_package(Bullet REQUIRED)
  set ( BULLET_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS}/.. )
  set (INCLUDES ${INCLUDES}
       ${BULLET_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${BULLET_LIBRARIES}
  )
endif(BLUB_USE_BULLET)

if(BLUB_USE_OGRE3D)
  find_package(OGRE REQUIRED)
  set (INCLUDES ${INCLUDES}
    ${OGRE_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${OGRE_LIBRARIES}
  )
endif(BLUB_USE_OGRE3D)

if(BLUB_USE_OIS)
  find_package(OIS REQUIRED)
  set (INCLUDES ${INCLUDES}
       ${OIS_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${OIS_LIBRARIES}
  )
endif(BLUB_USE_OIS)

if(BLUB_USE_CEF3)
  set( CEF3_ROOT "/usr/local" CACHE PATH "cef3 root" )
  find_package(Cef3 REQUIRED)
  set (INCLUDES ${INCLUDES}
    ${CEF3_INCLUDE_DIRS}
  )
  set (LIBS ${LIBS}
    ${CEF3_LIBRARIES}
  )
endif(BLUB_USE_CEF3)
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
if (BLUB_BUILD_SERIALIZATION)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} serialization)
endif (BLUB_BUILD_SERIALIZATION)
if (BLUB_BUILD_SIGNALS)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} signals)
endif (BLUB_BUILD_SIGNALS)
if (BLUB_BUILD_SYNC)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} sync)
endif (BLUB_BUILD_SYNC)

# additianal standard includes
set (INCLUDES ${INCLUDES}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/blub/core
  )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (BLUB_DEBUG 1)
    unset (BLUB_RELEASE)
else()
    set (BLUB_RELEASE 1)
    unset (BLUB_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

configure_file (
  "${PROJECT_SOURCE_DIR}/core/source/blub/core/globals.hpp.in"
  "${PROJECT_BINARY_DIR}/blub/core/globals.hpp"
  )

# a macro to prefix list of strings
macro (prefix_list prefix list result)
  foreach(to_prefix ${${list}})
    set(${result} ${${result}} "${prefix}${to_prefix}")
  endforeach(to_prefix)
endmacro (prefix_list)
  
# do libs
foreach(LIB_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD})
  unset(sources)
  unset(headers)
  unset(sources_with_prefix)
  unset(headers_with_prefix)
  add_subdirectory(${LIB_TO_BUILD})
  set (INCLUDES ${INCLUDES}
    ${PROJECT_SOURCE_DIR}/${LIB_TO_BUILD}/source
  )
  prefix_list(
    "${LIB_TO_BUILD}/source/blub/${LIB_TO_BUILD}/"
    sources
    sources_with_prefix
  )
  prefix_list(
    "${LIB_TO_BUILD}/source/blub/${LIB_TO_BUILD}/"
    headers
    headers_with_prefix
  )
# debug output
#  message("sources_with_prefix:${sources_with_prefix}")
#  message("headers_with_prefix:${headers_with_prefix}")
  add_library(${LIB_TO_BUILD} ${sources_with_prefix} ${headers_with_prefix})
  target_link_libraries (${LIB_TO_BUILD} ${LIBS})
  install (DIRECTORY ${PROJECT_SOURCE_DIR}/${LIB_TO_BUILD}/source/
	  DESTINATION include
	  FILES_MATCHING PATTERN "*.hpp")
endforeach(LIB_TO_BUILD)

# do executeables
foreach(EXEC_TO_BUILD ${EXAMPLE_EXECUTEABLES_TO_BUILD})
  unset(sources)
  unset(headers)
  unset(sources_with_prefix)
  unset(headers_with_prefix)
  add_subdirectory(examples/${EXEC_TO_BUILD})
  set (INCLUDES ${INCLUDES}
    ${PROJECT_SOURCE_DIR}/examples/${EXEC_TO_BUILD}/source
  )
  prefix_list(
    "examples/${EXEC_TO_BUILD}/source/"
    sources
    sources_with_prefix
  )
  prefix_list(
    "examples/${EXEC_TO_BUILD}/source/"
    headers
    headers_with_prefix
  )
  add_executable(${EXEC_TO_BUILD}-exec ${sources_with_prefix} ${headers_with_prefix})
  target_link_libraries(${EXEC_TO_BUILD}-exec ${BLUB_LIBRARIES_TO_BUILD} ${LIBS})
endforeach(EXEC_TO_BUILD)


include_directories(${INCLUDES})

install(FILES "${PROJECT_BINARY_DIR}/blub/core/globals.hpp"
        DESTINATION include/blub/core)
install(TARGETS ${BLUB_LIBRARIES_TO_BUILD}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static)

