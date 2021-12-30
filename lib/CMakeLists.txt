
set(HEADERS
    mytypes.hpp
    Engine.hpp
    OpenGLEngine.hpp
    shader.hpp
    mesh.hpp
    VulkanEngine.hpp
    Window.hpp
)

set(SOURCES
    makeOpengl.cpp
    makeVulkan.cpp
    Engine.cpp
    OpenGLEngine.cpp
    VulkanEngine.cpp
    Window.cpp
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


