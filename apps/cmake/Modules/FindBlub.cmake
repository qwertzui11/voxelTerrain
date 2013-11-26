# - Try to find blub
# Set BLUB_ROOT for search-hint
#
# Once done, this will define
# 
#  BLUB_FOUND 
#  BLUB_INCLUDE_DIRS 
#  BLUB_LIBRARIES - link these to use BLUB


IF(NOT EXISTS ${BLUB_ROOT})
  message(FATAL_ERROR "Could not find blub root-dir: ${BLUB_ROOT}")
endif ()
#IF(NOT EXISTS ${BLUB_BUILD_DIR})
#  message(FATAL_ERROR "Could not find blub build-dir: ${BLUB_BUILD_DIR}")
#endif()
#if (NOT EXISTS ${BLUB_SOURCE_DIR})
#  message(FATAL_ERROR "Could not find blub source-dir: ${BLUB_SOURCE_DIR}")
#endif()

message("BLUB_BUILD_DIR:${BLUB_BUILD_DIR}")

find_path(BLUB_GLOBALS_DIR NAMES blub/core/globals.hpp PATHS ${BLUB_ROOT} ${BLUB_ROOT}/include ${BLUB_BUILD_DIR} DOC "include path of globals.hpp")
# debug
# message("BLUB_GLOBALS_DIR:${BLUB_GLOBALS_DIR}")
set(BLUB_INCLUDE_DIRS ${BLUB_INCLUDE_DIRS}
  ${BLUB_GLOBALS_DIR}
)

if (NOT Blub_FIND_COMPONENTS)
  set(Blub_FIND_COMPONENTS
    async
    core
    graphic
    gui
    input
    math
    network
    physic
    procedural
    serialization
    signals
    sync
    web
  )
endif(NOT Blub_FIND_COMPONENTS)

FOREACH(comp ${Blub_FIND_COMPONENTS})
  find_path(BLUB_INCLUDE_DIR_${comp} 
    NAMES "blub/${comp}/predecl.hpp" 
    PATHS ${BLUB_ROOT} ${BLUB_ROOT}/ ${BLUB_SOURCE_DIR} 
    PATH_SUFFIXES /include/ "/${comp}/source/" "/apps/${comp}/source/"
    DOC "include directory of ${comp}")
  set(BLUB_INCLUDE_DIRS ${BLUB_INCLUDE_DIRS}
    ${BLUB_INCLUDE_DIR_${comp}}
  )
  find_library(Blub_LIBRARY_${comp}_RELEASE NAMES ${comp} PATHS ${BLUB_ROOT} ${BLUB_BUILD_DIR} PATH_SUFFIXES lib DOC "library ${comp} release")
  if (Blub_LIBRARY_${comp}_RELEASE)
    set (Blub_LIBRARY_${comp} "${Blub_LIBRARY_${comp}}"
      optimized "${Blub_LIBRARY_${comp}_RELEASE}"
    )
  endif()
  find_library(Blub_LIBRARY_${comp}_DEBUG NAMES "${comp}_d" PATHS ${BLUB_ROOT} ${BLUB_BUILD_DIR} PATH_SUFFIXES lib DOC "library ${comp} debug")
  if (Blub_LIBRARY_${comp}_DEBUG)
    set (Blub_LIBRARY_${comp} "${Blub_LIBRARY_${comp}}"
      debug "${Blub_LIBRARY_${comp}_DEBUG}"
    )
  endif()
  if(NOT Blub_LIBRARY_${comp})
  ELSE()
    SET(Blub_${comp}_FOUND 1)
  ENDIF()
  set(VARS_THAT_MUST_GET_FOUND ${VARS_THAT_MUST_GET_FOUND}
    BLUB_INCLUDE_DIR_${comp}
    Blub_LIBRARY_${comp}
  )
  set(BLUB_LIBRARIES ${BLUB_LIBRARIES}
    "${Blub_LIBRARY_${comp}}"
  )
  # debug
  # message("Blub_LIBRARY_${comp}:${Blub_LIBRARY_${comp}}")
  # message("BLUB_INCLUDE_DIR_${comp}:${BLUB_INCLUDE_DIR_${comp}}")
ENDFOREACH()


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  Blub DEFAULT_MSG BLUB_LIBRARIES BLUB_INCLUDE_DIRS BLUB_GLOBALS_DIR ${VARS_THAT_MUST_GET_FOUND}
)
