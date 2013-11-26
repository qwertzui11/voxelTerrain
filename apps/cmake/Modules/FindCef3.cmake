# - Try to find cef3
# Set CEF3_ROOT for search-hint
#
# Once done, this will define
# 
#  CEF3_FOUND 
#  CEF3_INCLUDE_DIRS 
#  CEF3_LIBRARIES - link these to use cef3


IF(NOT EXISTS ${CEF3_ROOT})
  message(FATAL_ERROR "Could not find cef3 root: ${CEF3_ROOT}")
endif ()

find_path(CEF3_INCLUDE_DIRS NAMES include/cef_version.h PATHS ${CEF3_ROOT} DOC "include path of cef3")

set (CEF3_INCLUDE_DIRS ${CEF3_INCLUDE_DIRS} ${CEF3_INCLUDE_DIRS}/include )

find_library(CEF3_LIBRARY_RELEASE NAMES cef PATHS ${CEF3_ROOT} PATH_SUFFIXES out/Release DOC "libcef release path")
find_library(CEF3_LIBRARY_DEBUG NAMES cef PATHS ${CEF3_ROOT} PATH_SUFFIXES out/Debug DOC "libcef debug path")
set(CEF3_LIBRARY 
  optimized ${CEF3_LIBRARY_RELEASE}
  debug ${CEF3_LIBRARY_DEBUG}
)
find_library(CEF3_LIBRARY_WRAPPER_RELEASE NAMES cef_dll_wrapper PATHS ${CEF3_ROOT} PATH_SUFFIXES out/Release/obj.target DOC "libcef-wrapper release path")
find_library(CEF3_LIBRARY_WRAPPER_DEBUG NAMES cef_dll_wrapper PATHS ${CEF3_ROOT} PATH_SUFFIXES out/Debug/obj.target DOC "libcef-wrapper debug path")
set(CEF3_LIBRARY_WRAPPER
  optimized ${CEF3_LIBRARY_WRAPPER_RELEASE}
  debug ${CEF3_LIBRARY_WRAPPER_DEBUG}
)

#  find_package(ATK REQUIRED) # already in gtk included
#  find_package(Cairo REQUIRED) # already in gtk included
#  find_package(GLIB REQUIRED) # already in gtk included
find_package(GTK2 REQUIRED gtk)
set (CEF3_INCLUDE_DIRS
      ${CEF3_INCLUDE_DIRS}
#       ${CAIRO_INCLUDE_DIRS}
#       ${ATK_INCLUDE_DIRS}
#       ${GLIB_INCLUDE_DIRS}
      ${GTK2_INCLUDE_DIRS}
)
set (CEF3_LIBRARIES 
  ${CEF3_LIBRARY}
  ${CEF3_LIBRARY_WRAPPER}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  CEF3
  REQUIRED_VARS CEF3_LIBRARY CEF3_LIBRARY_WRAPPER CEF3_INCLUDE_DIRS
)
