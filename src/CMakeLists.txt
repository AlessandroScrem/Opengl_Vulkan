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

   
set (data_dir "data")
# LOCAL DATA COPY
add_custom_command(TARGET app PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E rm -Rf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${data_dir}/
COMMAND ${CMAKE_COMMAND} -E copy_directory 
${CMAKE_CURRENT_SOURCE_DIR}/../${data_dir}/
$<TARGET_FILE_DIR:${PROJECT_NAME}>/${data_dir}
)
