# example macro
macro (add_example example_name)
  message( STATUS "Creating example ${example_name}" )
  set (INCLUDES ${INCLUDES}
    source
  )
  prefix_list(
    "source/"
    sources
    sources_with_prefix
  )
  prefix_list(
    "source/"
    headers
    headers_with_prefix
  )
  include_directories(${INCLUDES})

  # one example consists of one cpp. All other helper classes are header only.
  # Makes it easier to read the example, than rather jumping between several implementation files.
  # so every cpp has to have a main will become an executeable.
  foreach(MAIN_TO_BUILD ${sources_with_prefix})
    GET_FILENAME_COMPONENT(MAIN_TO_BUILD_NAME ${MAIN_TO_BUILD} NAME_WE)
    set(MAIN_TO_BUILD_RESULT_NAME "example-${example_name}-${MAIN_TO_BUILD_NAME}")
    add_executable(${MAIN_TO_BUILD_RESULT_NAME} ${MAIN_TO_BUILD} ${headers_with_prefix})
    target_link_libraries(${MAIN_TO_BUILD_RESULT_NAME} ${BLUB_LIBRARIES_TO_BUILD} ${LIBS})
    set_property(TARGET ${MAIN_TO_BUILD_RESULT_NAME} PROPERTY DEBUG_POSTFIX _d)
    if (BLUB_INSTALL_EXAMPLES)
      install (TARGETS ${MAIN_TO_BUILD_RESULT_NAME}
              DESTINATION bin)
    endif(BLUB_INSTALL_EXAMPLES)
  endforeach(MAIN_TO_BUILD)
endmacro (add_example)

option (BLUB_INSTALL_EXAMPLES "install examples" ON)

set(EXAMPLES_TO_BUILD ${EXAMPLES_TO_BUILD}
  voxelterrain
)

if (BLUB_BUILD_GRAPHIC)
  set(EXAMPLES_TO_BUILD ${EXAMPLES_TO_BUILD} graphic)  
endif (BLUB_BUILD_GRAPHIC)
if (BLUB_BUILD_PHYSIC)
  set(EXAMPLES_TO_BUILD ${EXAMPLES_TO_BUILD} physic)  
endif (BLUB_BUILD_PHYSIC)
if (BLUB_BUILD_WEB)
  set(EXAMPLES_TO_BUILD ${EXAMPLES_TO_BUILD} web)  
endif (BLUB_BUILD_WEB)

foreach(EXAMPLE_TO_BUILD ${EXAMPLES_TO_BUILD})
  add_subdirectory(${EXAMPLE_TO_BUILD})
endforeach(EXAMPLE_TO_BUILD)


