
add_library(vma INTERFACE)

add_library(stb_image STATIC)

add_library(tinyobjloader STATIC)

target_include_directories(vma INTERFACE vma)

target_sources(tinyobjloader PRIVATE 
  tinyobjloader/tiny_obj_loader.h
  tinyobjloader/tiny_obj_loader.cc
  )

target_sources(stb_image PRIVATE
    stb_image/stb_image.h
    stb_image/stb_image.cc
)   

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

target_include_directories(stb_image PUBLIC stb_image)