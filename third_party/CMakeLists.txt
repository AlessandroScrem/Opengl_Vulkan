
add_library(imgui_bindings STATIC)
target_sources(imgui_bindings PRIVATE 
  imgui_bindings/imgui_impl_glfw.cpp
  imgui_bindings/imgui_impl_glfw.h
  imgui_bindings/imgui_impl_opengl3.cpp
  imgui_bindings/imgui_impl_opengl3.h
  imgui_bindings/imgui_impl_opengl3_loader.h
  )
target_include_directories(imgui_bindings PUBLIC imgui_bindings)
target_link_libraries(imgui_bindings 
  PRIVATE 
      imgui::imgui 
      glfw::glfw
      )

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vma)


add_library(tinyobjloader STATIC)
target_sources(tinyobjloader PRIVATE 
  tinyobjloader/tiny_obj_loader.h
  tinyobjloader/tiny_obj_loader.cc
  )
target_include_directories(tinyobjloader PUBLIC tinyobjloader)

add_library(stb_image STATIC)
target_sources(stb_image PRIVATE
    stb_image/stb_image.h
    stb_image/stb_image.cc
)   
target_include_directories(stb_image PUBLIC stb_image)