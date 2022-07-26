message("\n---- Setup thrid party libraries ----")

set(third_party_folder "ThirdParty")

# Add Vulkan SDK
if(NOT TARGET vulkan)
    message("setup vulkan")
    set(VULKAN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vulkan)
    set(VULKAN_INCLUDE_DIR ${VULKAN_PATH}/include)
    file(GLOB VULKAN_HEADERS ${VULKAN_PATH}/include/vulkan/*.h)
    if(WIN32)
        set(VULKAN_LIB_DIR ${VULKAN_PATH}/lib/Win32)
        file(GLOB VULKAN_LIBS ${VULKAN_PATH}/lib/Win32/*.lib)
    endif(WIN32)

    add_library(vulkan INTERFACE ${VULKAN_HEADERS})   
    source_group("includes/vulkan" FILES ${VULKAN_HEADERS})
    
    target_include_directories(vulkan INTERFACE $<BUILD_INTERFACE:${VULKAN_INCLUDE_DIR}> $<INSTALL_INTERFACE:"3rdparty/vulkan">)
    foreach(VULKAN_LIB ${VULKAN_LIBS})
        target_link_libraries(vulkan INTERFACE ${VULKAN_LIB})
    endforeach()
    set_target_properties(vulkan PROPERTIES FOLDER ${third_party_folder}/vulkan)
endif()

# Add glfw window library
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()