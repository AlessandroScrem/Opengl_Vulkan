
set(HEADERS
    Window.hpp
    OpenGLWindow.hpp
    shader.hpp
    mesh.hpp
    VulkanWindow.hpp
)

set(SOURCES
    Window.cpp
    OpenGLWindow.cpp
    VulkanWindow.cpp
)

add_library(mylib ${SOURCES} ${HEADERS})

target_link_libraries(mylib
    PRIVATE 
        glfw::glfw
        GLEW::GLEW
        Vulkan::Vulkan
    PUBLIC 
        glm::glm
    )

target_link_options(mylib INTERFACE "/ignore:4099")

target_include_directories(mylib
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} 
 )


