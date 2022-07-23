
set(LIBNAME_CORE "core")

file(GLOB CORE_SOURCES "*.cpp" "*.hpp" "*.h" "${PROJECT_SOURCE_DIR}/3rdparty/imgui/*.cpp")
file(GLOB CORE_HEADERS "*.hpp" "*.h")

source_group("source" FILES ${CORE_SOURCES})
source_group("header" FILES ${CORE_HEADERS})

add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core vulkan glfw)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})