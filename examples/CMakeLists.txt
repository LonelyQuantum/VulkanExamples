# Build an example
set(examples_folder "Examples")

function(buildExample EXAMPLE_NAME)
    set(EXAMPLE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME})
    message(STATUS "Generating project file for example in ${EXAMPLE_FOLDER}")

    # Sources and headers
    file(GLOB SOURCES *.cpp ${BASE_HEADERS} ${EXAMPLE_FOLDER}/*.cpp)
    file(GLOB HEADERS *.cpp ${BASE_HEADERS} ${EXAMPLE_FOLDER}/*.h ${EXAMPLE_FOLDER}/*.hpp)
    source_group("source" FILES ${SOURCES})
    source_group("headers" FILES ${HEADERS})

    # Shaders sources
	set(SHADER_DIR_GLSL "../assets/shaders/glsl/${EXAMPLE_NAME}")
	file(GLOB SHADERS_GLSL "${SHADER_DIR_GLSL}/*.vert" "${SHADER_DIR_GLSL}/*.frag" "${SHADER_DIR_GLSL}/*.comp" "${SHADER_DIR_GLSL}/*.geom" "${SHADER_DIR_GLSL}/*.tesc" "${SHADER_DIR_GLSL}/*.tese" "${SHADER_DIR_GLSL}/*.mesh" "${SHADER_DIR_GLSL}/*.task" "${SHADER_DIR_GLSL}/*.rgen" "${SHADER_DIR_GLSL}/*.rchit" "${SHADER_DIR_GLSL}/*.rmiss" "${SHADER_DIR_GLSL}/*.rcall")
	set(SHADER_DIR_HLSL "../assets/shaders/hlsl/${EXAMPLE_NAME}")
	file(GLOB SHADERS_HLSL "${SHADER_DIR_HLSL}/*.vert" "${SHADER_DIR_HLSL}/*.frag" "${SHADER_DIR_HLSL}/*.comp" "${SHADER_DIR_HLSL}/*.geom" "${SHADER_DIR_HLSL}/*.tesc" "${SHADER_DIR_HLSL}/*.tese" "${SHADER_DIR_HLSL}/*.mesh" "${SHADER_DIR_HLSL}/*.task" "${SHADER_DIR_HLSL}/*.rgen" "${SHADER_DIR_HLSL}/*.rchit" "${SHADER_DIR_HLSL}/*.rmiss" "${SHADER_DIR_HLSL}/*.rcall")
	source_group("shaders/glsl" FILES ${SHADERS_GLSL})
	source_group("shaders/hlsl" FILES ${SHADERS_HLSL})

    file(GLOB README_FILES "${EXAMPLE_FOLDER}/*.md") # readme.md

    # Add target
    add_executable(${EXAMPLE_NAME} ${SOURCES} ${HEADERS} ${SHADERS_GLSL} ${SHADERS_HLSL} ${README_FILES})
    target_link_libraries(${EXAMPLE_NAME} core)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER ${examples_folder})

    # Installation settings
    file(GLOB SHADERS_GLSL_SPV "${SHADER_DIR_GLSL}/*.spv")
    file(GLOB SHADERS_HLSL_SPV "${SHADERS_DIR_HLSL}/*.spv")
    install(TARGETS ${EXAMPLE_NAME} DESTINATION "examples")
    install(FILES ${SHADERS_GLSL_SPV} DESTINATION "assets/shaders/glsl/${EXAMPLE_NAME}")
    install(FILES ${SHADERS_HLSL_SPV} DESTINATION "assets/shaders/hlsl/${EXAMPLE_NAME}")
endfunction(buildExample)

# Build all examples
function(buildExamples)
	foreach(EXAMPLE ${EXAMPLES})
		buildExample(${EXAMPLE})
	endforeach(EXAMPLE)
endfunction(buildExamples)



set(EXAMPLES
	triangle
)

buildExamples()