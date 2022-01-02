
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
    vk/VulkanCommandBuffer.hpp
    vk/VulkanCommandBuffer.cpp
)
    
set(OGL_SRC
    ogl/OpenGLEngine.hpp
    ogl/makeOpengl.cpp
    ogl/OpenGLEngine.cpp
)
    
set(COMMON_SRC
    common/Window.hpp
    common/Window.cpp
    common/shader.hpp
    common/mesh.hpp
)
set(PUB_SRC
    Engine.hpp
    Engine.cpp
    mytypes.hpp
)

add_library(mylib  ${COMMON_SRC} ${PUB_SRC} ${OGL_SRC} ${VK_SRC})

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


