cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(PROJNAME VulkanExamples)

project(${PROJNAME} VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)


# ---- Setup MSVC compile options ----
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    add_compile_options("/utf-8") # Avoid bug for Chinese comments
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# ---- Disable in-source builds ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Set exe files ---
if(WIN32)
  set(glslangValidator_exe ${CMAKE_SOURCE_DIR}/3rdparty/VulkanSDK/bin/Win32/glslangValidator.exe)
  set(glslc_exe ${CMAKE_SOURCE_DIR}/3rdparty/VulkanSDK/bin/Win32/glslc.exe) 
endif(WIN32)

# ---- Setup configure header ----
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
configure_file(configFile.h.in configFile.h)
install(FILES "${PROJECT_BINARY_DIR}/configFile.h" DESTINATION "include")

add_subdirectory(3rdparty)
add_subdirectory(core)
add_subdirectory(examples)