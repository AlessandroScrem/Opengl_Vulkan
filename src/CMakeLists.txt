project(app)

add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE  
        engine_lib
)

target_compile_definitions(app PUBLIC ${ENGINE})
message(STATUS "ENGINE = " ${ENGINE})

# fix bin target directory to project/buld/debug|release
set_target_properties( app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

message(STATUS "TEST_APP = " ${TEST_APP})

if( ${TEST_APP})
    add_test(
            NAME vulkan 
        COMMAND $<TARGET_FILE:app> --vulkan
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )

    add_test(
            NAME opengl 
        COMMAND $<TARGET_FILE:app> --opengl
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )   

    set_tests_properties(vulkan PROPERTIES
        FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
    )
endif()


# SHADER COMPILATION TO SPIRV

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()


set (DATA_REL_DIR "../data")
get_filename_component(DATA_DIR
                       "${CMAKE_CURRENT_LIST_DIR}/${DATA_REL_DIR}"
                       ABSOLUTE)

set (SHADERS_DIR "${DATA_DIR}/shaders")
                       
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADERS_DIR}/*.vert"
    "${SHADERS_DIR}/*.frag"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME  ${GLSL} NAME)
  set(SPIRV "${SHADERS_DIR}/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)
add_dependencies(app Shaders)

# LOCAL DATA COPY

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -Rf "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${DATA_DIR}/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
)