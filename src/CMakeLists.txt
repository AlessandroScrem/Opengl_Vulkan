project(app)

add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE  
        mylib
)


set_target_properties( app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

set (sh_dir "shaders")

# LOCAL DATA COPY
add_custom_command(TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -Rf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${sh_dir}/
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/vk/${sh_dir}/
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${sh_dir}
)