
add_library(vma INTERFACE)

# add_library(stb_image INTERFACE)

# add_library(tinyobjloader STATIC)

#both vma and glm and header only libs so we only need the include path
target_include_directories(vma INTERFACE vma)

# target_sources(tinyobjloader PRIVATE 
#   tinyobjloader/tiny_obj_loader.h
#   tinyobjloader/tiny_obj_loader.cc
#   )

# target_include_directories(tinyobjloader PUBLIC tinyobjloader)


# target_include_directories(stb_image INTERFACE stb_image)