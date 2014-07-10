cmake_minimum_required (VERSION 2.8)
project (libblub)

# The version number.
set (blub_VERSION_MAJOR 1)
set (blub_VERSION_MINOR 0)

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11 -Wno-unused-variable ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
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
#option (BLUB_USE_CEF3 "use cef3" ON)
option (BLUB_USE_OGRE3D "use ogre3d" ON)
option (BLUB_USE_OIS "use ois" ON)
#option (BLUB_USE_SOCI "use soci" ON)
#option (BLUB_USE_GTEST "use gtest" ON)

unset (LIBS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


# find boost
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK )
set( BOOST_ROOT "/usr/local" CACHE PATH "boost root")
IF(NOT EXISTS ${BOOST_ROOT})
  message(FATAL_ERROR "Could not find boost root: ${BOOST_ROOT}")
endif ()
find_package(Boost 1.55.0 REQUIRED COMPONENTS log log_setup system thread chrono date_time iostreams serialization )
set (INCLUDES ${INCLUDES}
  ${Boost_INCLUDE_DIRS}
)
set (LIBS ${LIBS}
  ${Boost_LIBRARIES}
)

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
if (BLUB_BUILD_SERIALIZATION)
  set( BLUB_LIBRARIES_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD} serialization)
endif (BLUB_BUILD_SERIALIZATION)
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
)

set(EXAMPLE_EXECUTEABLES_TO_BUILD ${EXAMPLE_EXECUTEABLES_TO_BUILD}
  voxelterrain
)


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

# do examples
if (BLUB_BUILD_EXAMPLES)
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
  # one example consists of one cpp. All other helper classes are header only.
  # Makes it easier to read the example, than rather jumping between several implementation files.
  # so every cpp has to have a main will become an executeable.
  foreach(MAIN_TO_BUILD ${sources_with_prefix})
    GET_FILENAME_COMPONENT(MAIN_TO_BUILD_NAME ${MAIN_TO_BUILD} NAME_WE)
    set(MAIN_TO_BUILD_RESULT_NAME "example-${EXEC_TO_BUILD}-${MAIN_TO_BUILD_NAME}-exec")
    add_executable(${MAIN_TO_BUILD_RESULT_NAME} ${MAIN_TO_BUILD} ${headers_with_prefix})
    target_link_libraries(${MAIN_TO_BUILD_RESULT_NAME} ${BLUB_LIBRARIES_TO_BUILD} ${LIBS})
  endforeach(MAIN_TO_BUILD)
endforeach(EXEC_TO_BUILD)
endif(BLUB_BUILD_EXAMPLES)

# do tests
if (BLUB_BUILD_TESTS)
  unset(sources)
  unset(headers)
  unset(sources_with_prefix)
  unset(headers_with_prefix)
  add_subdirectory(tests)
  set (INCLUDES ${INCLUDES}
    ${PROJECT_SOURCE_DIR}/tests/source
  )
  prefix_list(
    "tests/source/"
    sources
    sources_with_prefix
  )
  prefix_list(
    "tests/source/"
    headers
    headers_with_prefix
  )
  add_executable(tests-exec ${sources_with_prefix} ${headers_with_prefix})
  target_link_libraries(tests-exec ${BLUB_LIBRARIES_TO_BUILD} ${LIBS} ${GTEST_BOTH_LIBRARIES})
endif(BLUB_BUILD_TESTS)

include_directories(${INCLUDES})

install(FILES "${PROJECT_BINARY_DIR}/blub/core/globals.hpp"
        DESTINATION include/blub/core)
install(TARGETS ${BLUB_LIBRARIES_TO_BUILD}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static)

# message("libs:${LIBS}")

