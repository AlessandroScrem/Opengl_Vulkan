project(app)

add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE  
        mylib
)


target_compile_definitions(app PUBLIC ${ENGINE})
message(STATUS "ENGINE = " ${ENGINE})

# fix bin target directory to project/buld/debug|release
set_target_properties( app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_test(
         NAME vulkan 
     COMMAND $<TARGET_FILE:app> --vulkan
     WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
     )

add_test(
         NAME opengl 
     COMMAND $<TARGET_FILE:app> --opengl
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     )   

set_tests_properties(vulkan PROPERTIES
     FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
   )


   
set (sh_dir "shaders")
# LOCAL DATA COPY
add_custom_command(TARGET app PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E rm -Rf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${sh_dir}/
COMMAND ${CMAKE_COMMAND} -E copy_directory 
${CMAKE_CURRENT_SOURCE_DIR}/../lib/vk/${sh_dir}/
$<TARGET_FILE_DIR:${PROJECT_NAME}>/${sh_dir}
)
set (tex_dir "textures")
# LOCAL DATA COPY
add_custom_command(TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -Rf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${tex_dir}/
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/vk/${tex_dir}/
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${tex_dir}
)