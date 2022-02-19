
add_executable(Test camera.cpp)

target_link_libraries(Test 
    PRIVATE  
        mylib
)

add_test(NAME CameraTest COMMAND ${TARGETFILE}Test)

