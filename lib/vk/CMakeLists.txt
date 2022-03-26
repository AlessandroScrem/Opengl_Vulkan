
add_library(vk_lib "")

target_sources(vk_lib
    PRIVATE
        vktypes.h
        vk_initializers.h
        vk_initializers.cpp
        VulkanEngine.hpp
        makeVulkan.cpp
        VulkanEngine.cpp
        VulkanDevice.hpp
        VulkanDevice.cpp
        VulkanSwapchain.hpp
        VulkanSwapchain.cpp
        VulkanPipeline.hpp
        VulkanPipeline.cpp
        VulkanVertexBuffer.hpp
        VulkanVertexBuffer.cpp
        VulkanImage.hpp
        VulkanImage.cpp
        VulkanUbo.hpp
        VulkanUbo.cpp
        VulkanShader.hpp
        VulkanShader.cpp
)

target_link_libraries(vk_lib 
    PRIVATE 
        common_lib 
        spdlog::spdlog
        glm::glm
        glfw::glfw 
        glew::glew
        Vulkan::Vulkan
        shaderc::shaderc #shader online compilation
        glslang::SPIRV   #shader online compilation
        stb_image
        vma
)

target_include_directories(vk_lib 
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE 
)