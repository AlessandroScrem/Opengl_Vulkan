add_library(ogl_lib "")

target_sources(ogl_lib
    PRIVATE
        OpenGLEngine.hpp
        makeOpengl.cpp
        OpenGLEngine.cpp
        InitOpengl.cpp
        OpenglVertexBuffer.hpp
        OpenglVertexBuffer.hpp
        OpenglImage.hpp
        OpenglImage.cpp
        OpenglShader.hpp
        OpenglUbo.hpp
        OpenglObj.hpp
        shader_constants.h
)


target_link_libraries(ogl_lib
    PRIVATE 
        glfw::glfw 
        glew::glew
        Vulkan::Vulkan
        stb
    PUBLIC 
        common_lib
        glm::glm
        spdlog::spdlog
    )

target_include_directories(ogl_lib 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/..
)