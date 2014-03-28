#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIRS - the Assimp include directories
#  ASSIMP_LIBRARIES - link these to use Assimp

include(FindPkgMacros)
findpkg_begin(ASSIMP)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(ASSIMP_HOME)

# construct search paths
set(ASSIMP_PREFIX_PATH ${ASSIMP_HOME} ${ENV_ASSIMP_HOME} /usr/local /usr/local/include /usr/local/lib /usr/include /usr/lib /usr/local/include/assimp /usr/include/assimp /usr/lib/assimp /usr/local/lib/assimp)
create_search_paths(ASSIMP)
# redo search if prefix path changed
clear_if_changed(ASSIMP_PREFIX_PATH
  ASSIMP_LIBRARY_REL
  ASSIMP_LIBRARY_DBG
  ASSIMP_INCLUDE_DIR
)

set(ASSIMP_LIBRARY_NAMES assimp)
get_debug_names(ASSIMP_LIBRARY_NAMES)

#use_pkgconfig(ASSIMP_PKGC ASSIMP)
use_pkgconfig(ASSIMP assimp)

findpkg_framework(ASSIMP)

find_path(ASSIMP_INCLUDE_DIR NAMES config.h HINTS ${ASSIMP_INC_SEARCH_PATH} ${ASSIMP_PKGC_INCLUDE_DIRS} PATH_SUFFIXES assimp)
find_library(ASSIMP_LIBRARY_REL NAMES ${ASSIMP_LIBRARY_NAMES} HINTS ${ASSIMP_LIB_SEARCH_PATH} ${ASSIMP_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" release relwithdebinfo minsizerel)
find_library(ASSIMP_LIBRARY_DBG NAMES ${ASSIMP_LIBRARY_NAMES_DBG} HINTS ${ASSIMP_LIB_SEARCH_PATH} ${ASSIMP_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" debug)
make_library_set(ASSIMP_LIBRARY)

findpkg_finish(ASSIMP)
add_parent_dir(ASSIMP_INCLUDE_DIRS ASSIMP_INCLUDE_DIR)

