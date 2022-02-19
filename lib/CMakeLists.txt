
set(VK_SRC
    vk/VulkanEngine.hpp
    vk/makeVulkan.cpp
    vk/VulkanEngine.cpp
    vk/VulkanDevice.hpp
    vk/VulkanDevice.cpp
    vk/VulkanSwapchain.hpp
    vk/VulkanSwapchain.cpp
    vk/VulkanPipeline.hpp
    vk/VulkanPipeline.cpp
    vk/VulkanVertexBuffer.hpp
    vk/VulkanVertexBuffer.cpp
    vk/VulkanImage.hpp
    vk/VulkanImage.cpp
    vk/VulkanUbo.hpp
    vk/VulkanUbo.cpp
)
    
set(OGL_SRC
    ogl/OpenGLEngine.hpp
    ogl/makeOpengl.cpp
    ogl/OpenGLEngine.cpp
    ogl/InitOpengl.cpp
    ogl/OpenglVertexBuffer.hpp
    ogl/OpenglVertexBuffer.hpp
    ogl/OpenglImage.hpp
    ogl/OpenglImage.cpp
    ogl/OpenglShader.hpp
    ogl/OpenglUbo.hpp
)
    
set(COMMON_SRC
    common/Window.hpp
    common/Window.cpp
    common/vertex.h
    common/model.hpp
    common/model.cpp
    common/camera.hpp
    common/camera.cpp
    
    common/input/utils.hpp
    common/input/input_key.hpp
    common/input/input_devices.hpp
    common/input/input_manager.hpp
    common/input/input_manager.cpp
    common/input/multiplatform_input.hpp
    common/input/multiplatform_input.cpp
    common/input/service_locator.hpp
)
set(PUB_SRC
    Engine.hpp
    Engine.cpp
    mytypes.hpp
    ngn_command.hpp
)

add_library(mylib  ${COMMON_SRC} ${PUB_SRC} ${OGL_SRC} ${VK_SRC})

target_link_libraries(mylib
    PRIVATE 
        glfw::glfw
        glew::glew
        Vulkan::Vulkan
    PUBLIC 
        glm::glm
        spdlog::spdlog
    )

target_link_options(mylib INTERFACE "/ignore:4099")

target_include_directories(mylib
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        stb
        tinyobjloader  
 )




