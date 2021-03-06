add_library(ogl_lib "")

target_sources(ogl_lib
    PRIVATE
        OpenGLEngine.hpp
        makeOpengl.cpp
        OpenGLEngine.cpp
        InitOpengl.cpp
        OpenglVertexBuffer.hpp
        OpenglVertexBuffer.cpp
        OpenglImage.hpp
        OpenglImage.cpp
        OpenglShader.hpp
        OpenglShader.cpp
        OpenglUbo.hpp
        OpenglUIOverlay.h
        OpenglUIOverlay.cpp
)


target_link_libraries(ogl_lib
    PRIVATE 
        glfw::glfw 
        glew::glew
        Vulkan::Vulkan
        stb_image
        common_lib
        glm::glm
        spdlog::spdlog
        imgui::imgui
        imgui_bindings
    )

target_include_directories(ogl_lib 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE 
)