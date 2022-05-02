
add_library(common_lib "") 

target_sources(common_lib 
    PRIVATE 
        Window.hpp
        Window.cpp
        vertex.h
        model.hpp
        model.cpp
        camera.hpp
        camera.cpp
        mytypes.hpp
        glsl_constants.h

        input/utils.hpp
        input/input_key.hpp
        input/input_devices.hpp
        input/input_manager.hpp
        input/input_manager.cpp
        input/multiplatform_input.hpp
        input/multiplatform_input.cpp
        input/service_locator.hpp
)

target_link_libraries(common_lib
    PRIVATE 
        glfw::glfw 
        glew::glew 
        spdlog::spdlog
        tinyobjloader
        imgui::imgui
    PUBLIC
        glm::glm
    )

target_include_directories(common_lib 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/input
    PRIVATE
)

