
set(HEADERS
    OpenGLWindow.hpp
)

set(SOURCES
    OpenGLWindow.cpp
)

add_library(mylib ${SOURCES} ${HEADERS})

target_link_libraries(mylib
    PRIVATE 
        glfw::glfw
        GLEW::GLEW 
    PUBLIC 
        glm::glm
    )

target_link_options(mylib INTERFACE "/ignore:4099")

target_include_directories(mylib
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} 
 )


