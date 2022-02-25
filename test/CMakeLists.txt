
set(all_tests
    test_camera.cpp
    test_utils.cpp
)

add_executable(Test ${all_tests})


target_link_libraries(Test 
    PRIVATE  
        engine_lib
)

add_test(NAME Test COMMAND ${TARGETFILE}Test)


