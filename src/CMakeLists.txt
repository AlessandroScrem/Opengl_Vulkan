add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE  
        engine_lib
)

target_compile_definitions(app PUBLIC ${ENGINE})
message(NOTICE "ENGINE = " ${ENGINE})

# fix bin target directory to project/buld/debug|release
set_target_properties( app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# add test suite 
if( ${TEST_APP})
    add_test(
            NAME vulkan 
        COMMAND $<TARGET_FILE:app> --vulkan
        WORKING_DIRECTORY $<TARGET_FILE_DIR:app>
        )

    add_test(
            NAME opengl 
        COMMAND $<TARGET_FILE:app> --opengl
        WORKING_DIRECTORY $<TARGET_FILE_DIR:app>
        )   
        
    set_tests_properties(vulkan PROPERTIES FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed")
    set_tests_properties(opengl PROPERTIES FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed")
endif()

# copy local data to target app dir
set(DATA_SRC  "${CMAKE_CURRENT_LIST_DIR}/../data/")
set(DATA_DEST "$<TARGET_FILE_DIR:app>/data")
add_custom_command(TARGET app POST_BUILD
    COMMENT "Copying data to bin dir ..."
    COMMAND ${CMAKE_COMMAND} -E rm -Rf ${DATA_DEST}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SRC} ${DATA_DEST}
    VERBATIM
)