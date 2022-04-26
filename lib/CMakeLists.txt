

add_subdirectory(common)
add_subdirectory(ogl)
add_subdirectory(vk)

add_library(engine_lib  "")

target_sources(engine_lib 
    PRIVATE 
        Engine.hpp
        Engine.cpp
        ngn_command.hpp
)

target_link_libraries(engine_lib
    PRIVATE 
        stb_image
    PUBLIC 
        common_lib
        vk_lib
        ogl_lib
        glm::glm
        spdlog::spdlog
    )
# remove warning LNK4099: impossibile trovare il PDB 'glfw.pdb 
target_link_options(engine_lib INTERFACE "/ignore:4099")

target_include_directories(engine_lib
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
 )







