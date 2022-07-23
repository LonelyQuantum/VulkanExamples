#------------------------------------------------------------------------------------
# Find dependencies for GLSL files (#include ...)
# Call 'glslc -M' to find all dependencies of the file and return the list
# in GLSL_DEPENDENCY
#
function(get_glsl_dependecies _SRC _FLAGS GLSLC)
   
  get_filename_component(FILE_NAME ${_SRC} NAME)
  get_filename_component(DIR_NAME ${_SRC} DIRECTORY)

  message(STATUS " - Find dependencies for ${FILE_NAME}")
  #message(STATUS "calling : ${GLSLC} ${_FLAGS} -M ${_SRC} OUTPUT_VARIABLE DEP RESULT_VARIABLE RES")
  separate_arguments(_FLAGS)
  execute_process(COMMAND ${GLSLC} ${_FLAGS} -M ${_SRC} OUTPUT_VARIABLE DEP RESULT_VARIABLE RES )
  if(RES EQUAL 0)
    # Removing "name.spv: "
    string(REGEX REPLACE "[^:]*: " "" DEP ${DEP})
    # Splitting each path with a ';' 
    string(REPLACE " ${DIR_NAME}"  ";${DIR_NAME}" DEP ${DEP})
    set(GLSL_DEPENDENCY ${DEP} PARENT_SCOPE)
  endif()
endfunction()

#------------------------------------------------------------------------------------
# Function to compile all GLSL source files to Spir-V
#
# SHADER_SOURCE_FILES : All sources to compile
# SHADER_HEADER_FILES : Dependencie header files
# DST : The destination directory (need to be absolute)
# VULKAN_TARGET : to define the vulkan target i.e vulkan1.2 (default vulkan1.1)
# HEADER ON: if ON, will generate headers instead of binary Spir-V files
# DEPENDENCY : ON|OFF will create the list of dependencies for the GLSL source file
# 
# compile_glsl(
#   SOURCES_FILES foo.vert foo.frag
#   DST ${CMAKE_CURRENT_SOURCE_DIR}/shaders
#   FLAGS -g0
# )
#
function(compile_glsl)
  set(oneValueArgs DST VULKAN_TARGET HEADER DEPENDENCY FLAGS)
  set(multiValueArgs SHADER_SOURCE_FILES SHADER_HEADER_FILES)
  cmake_parse_arguments(COMPILE  "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  # Check if the GLSL compiler is present
  if(NOT GLSLANGVALIDATOR)
    message(ERROR "Could not find GLSLANGVALIDATOR to compile shaders")
    return()
  endif()

  # By default use Vulkan 1.2
  if(NOT DEFINED COMPILE_VULKAN_TARGET)
    set(COMPILE_VULKAN_TARGET vulkan1.2)
  endif()

  # If destination is not present, return
  if(NOT DEFINED COMPILE_DST)
    message(ERROR " --- DST not defined")
    return()
  endif()

  # Make the output directory if needed
  file(MAKE_DIRECTORY ${COMPILE_DST})

  # If no flag set -g (debug)
  if(NOT DEFINED COMPILE_FLAGS)
    set(COMPILE_FLAGS -g)
  endif()

  # Search for dependency by default
  if(NOT DEFINED COMPILE_DEPENDENCY)
    set(COMPILE_DEPENDENCY ON)
  endif()

  separate_arguments(_FLG UNIX_COMMAND ${COMPILE_FLAGS})

  # Compiling all GLSL sources
  foreach(GLSL_SRC ${COMPILE_SHADER_SOURCE_FILES})

    # Find the dependency files for the GLSL source
    # or use all headers as dependencies.
    if(COMPILE_DEPENDENCY)
        get_glsl_dependecies(${GLSL_SRC} ${COMPILE_FLAGS} ${GLSLC})
    else()
      set(GLSL_DEPENDENCY ${COMPILE_SHADER_HEADER_FILES}) 
    endif()

    # Default compiler command, always adding debug information (Add and option to opt-out?)
    set(COMPILE_CMD  ${_FLG} --target-env ${COMPILE_VULKAN_TARGET})

    # Compilation to headers need a variable name, the output will be a .h
    get_filename_component(FILE_NAME ${GLSL_SRC} NAME)
    if(COMPILE_HEADER)           
        STRING(REPLACE "." "_" VAR_NAME ${FILE_NAME}) # Name of the variable in the header
        list(APPEND COMPILE_CMD  --vn ${VAR_NAME})
        set(GLSL_OUT "${COMPILE_DST}/${FILE_NAME}.h")
    else()
        set(GLSL_OUT "${COMPILE_DST}/${FILE_NAME}.spv")
        list(APPEND _SPVS ${GLSL_OUT})
    endif() 


    # Appending the output name and the file source
    list(APPEND COMPILE_CMD -o ${GLSL_OUT} ${GLSL_SRC} )

    # The custom command is added to the build system, check for the presence of the output
    # but also for changes done in GLSL headers 
    add_custom_command(
         PRE_BUILD
         OUTPUT ${GLSL_OUT}
         COMMAND echo ${GLSLANGVALIDATOR} ${COMPILE_CMD}
         COMMAND ${GLSLANGVALIDATOR} ${COMPILE_CMD}
         MAIN_DEPENDENCY ${GLSL_SRC}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         DEPENDS ${GLSL_DEPENDENCY}
      )
  endforeach()
endfunction()

#------------------------------------------------------------------------------------
# Function to compile all GLSL files from a source to Spir-V
# The sources are all .vert, .frag, .r*  and the headers for the source are .glsl and .h
# This allows to modify one of the header and getting the sources recompiled.
#
# SRC : The directory source of the shaders
# DST : The destination directory (need to be absolute)
# VULKAN_TARGET : to define the vulkan target i.e vulkan1.2 (default vulkan1.1)
# HEADER ON: if present, will generate headers instead of binary Spir-V files
# DEPENDENCY : ON|OFF will create the list of dependencies for the GLSL source file 
# FLAGS : other glslValidator flags 
#
# compile_glsl_directory(
#    SRC "${CMAKE_CURRENT_SOURCE_DIR}/shaders" 
#    DST "${CMAKE_CURRENT_SOURCE_DIR}/autogen" 
#    VULKAN_TARGET "vulkan1.2"
#    HEADER ON
#    )
#
function(compile_glsl_directory)
  set(oneValueArgs SRC DST VULKAN_TARGET HEADER DEPENDENCY FLAGS)
  set(multiValueArgs)
  cmake_parse_arguments(COMPILE  "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Collecting all source files
  file(GLOB GLSL_SOURCE_FILES
    "${COMPILE_SRC}/*.comp"     # Compute
    "${COMPILE_SRC}/*.frag"     # Fragment
    "${COMPILE_SRC}/*.geom"     # Geometry
    "${COMPILE_SRC}/*.mesh"     # Mesh
    "${COMPILE_SRC}/*.rahit"    # Ray any hit
    "${COMPILE_SRC}/*.rcall"    # Ray callable
    "${COMPILE_SRC}/*.rchit"    # Ray closest hit
    "${COMPILE_SRC}/*.rgen"     # Ray generation
    "${COMPILE_SRC}/*.rint"     # Ray intersection
    "${COMPILE_SRC}/*.rmiss"    # Ray miss
    "${COMPILE_SRC}/*.task"     # Task
    "${COMPILE_SRC}/*.tesc"     # Tessellation control
    "${COMPILE_SRC}/*.tese"     # Tessellation evaluation
    "${COMPILE_SRC}/*.vert"     # Vertex
    )

  # Collecting headers for dependencies
  file(GLOB GLSL_HEADER_FILES
    "${COMPILE_SRC}/*.glsl"     # Auto detect - used for header
    "${COMPILE_SRC}/*.h"
    )

  # By default use Vulkan 1.1
  if(NOT DEFINED COMPILE_VULKAN_TARGET)
    set(COMPILE_VULKAN_TARGET vulkan1.1)
  endif()

  # If destination is not present, same as source
  if(NOT DEFINED COMPILE_DST)
    set(COMPILE_DST ${COMPILE_SRC})
  endif()

  # If no flag set -g (debug)
  if(NOT DEFINED COMPILE_FLAGS)
    set(COMPILE_FLAGS -g)
  endif()

  # Compiling all GLSL
  compile_glsl(SHADER_SOURCE_FILES ${GLSL_SOURCE_FILES} 
               SHADER_HEADER_FILES ${GLSL_HEADER_FILES}  
               DST ${COMPILE_DST} 
               VULKAN_TARGET ${COMPILE_VULKAN_TARGET} 
               HEADER ${COMPILE_HEADER}
               DEPENDENCY ${COMPILE_DEPENDENCY}
               FLAGS ${COMPILE_FLAGS}
               )
endfunction()
