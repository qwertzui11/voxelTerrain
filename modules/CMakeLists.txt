# module macro
macro (add_module module_name)
  if (sources)
    message( STATUS "Creating module ${module_name}" )
    prefix_list(
      "source/blub/${module_name}/"
      sources
      sources_with_prefix
    )
    prefix_list(
      "source/blub/${module_name}/"
      headers
      headers_with_prefix
    )
    # export.hpp TODO this generation sucks make own.
    # http://gcc.gnu.org/wiki/Visibility
    string(TOUPPER ${module_name} module_name_upper)
    set(header_export "${PROJECT_BINARY_DIR}/config_files/blub/${module_name}/export.hpp")
    include_directories(${INCLUDES})
    add_library(${module_name} ${sources_with_prefix} ${headers_with_prefix} )
    target_link_libraries (${module_name} ${modules_to_link} ${LIBS})
    GENERATE_EXPORT_HEADER( ${module_name}
				  BASE_NAME BLUB_${module_name_upper}
				  EXPORT_MACRO_NAME BLUB_${module_name_upper}_API
				  EXPORT_FILE_NAME ${header_export}
				  NO_EXPORT_MACRO_NAME BLUB_${module_name_upper}_LOCAL
				  STATIC_DEFINE BLUB_${module_name_upper}_BUILD_STATIC )
    install (DIRECTORY source/
	    DESTINATION include
	    FILES_MATCHING PATTERN "*.hpp")
    install (TARGETS ${module_name}
	    DESTINATION lib)
  endif(sources)
endmacro (add_module)

# modules
foreach(LIB_TO_BUILD ${BLUB_LIBRARIES_TO_BUILD})
  add_subdirectory(${LIB_TO_BUILD})
endforeach(LIB_TO_BUILD)
