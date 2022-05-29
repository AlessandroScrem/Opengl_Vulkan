
add_library(vk_lib "")

target_sources(vk_lib
    PRIVATE
        vktypes.h
        vktools.cpp
        vk_initializers.h
        vk_initializers.cpp
        VulkanEngine.hpp
        makeVulkan.cpp
        VulkanEngine.cpp
        VulkanDevice.hpp
        VulkanDevice.cpp
        VulkanSwapchain.hpp
        VulkanSwapchain.cpp
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
        imgui::imgui
        imgui_bindings
)

target_include_directories(vk_lib 
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE 
)
# Check if glslang validator is found
if(NOT  Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
  message(FATAL_ERROR "GLSLANG_VALIDATOR_EXECUTABLE not found")
endif()

# set shader dir
get_filename_component(SHADERS_DIR "${PROJECT_SOURCE_DIR}/data/shaders" ABSOLUTE )
file(GLOB_RECURSE GLSL_SOURCE_FILES
"${SHADERS_DIR}/*.frag"
"${SHADERS_DIR}/*.vert"
)

# copile glsl to spv
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${SHADERS_DIR}/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMENT "Compyiling gls to spv ..."
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

# make vk_lib dependent from Shaders
add_dependencies(vk_lib Shaders)

