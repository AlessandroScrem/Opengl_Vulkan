project(app)

add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE  
        mylib
)


