#include "OpenglImage.hpp"
// common lib
#include "mytypes.hpp"
// stb lib    
#include <stb_image.h>
// std
#include <string>
#include <iostream>

OpenglImage::OpenglImage(const std::string  &filename/*  = "data/textures/viking_room.png" */)
{
    SPDLOG_DEBUG("constructor"); 

    int width, height, nrComponents;

    const int alignement = 1;
    const int xoffset = 0;
    const int yoffset = 0;
    const int level_of_detail = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *pixels = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (!pixels){
        throw std::runtime_error("failed to load texture image!");
    }
    
    GLenum format{};
    GLenum internalformat{};
    if (nrComponents == 1){
        format = GL_RED;
        internalformat = GL_R8;
    }else if (nrComponents == 3){
        format = GL_RGB;
        internalformat = GL_RGB8;
    }else if (nrComponents == 4){ 
        format = GL_RGBA;
        internalformat = GL_RGBA8;
    }

    glCreateTextures(GL_TEXTURE_2D, num_of_textures, &textureID);
    glTextureParameteri(textureID, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
    glTextureStorage2D(textureID, num_of_textures, internalformat, width, height);
    glTextureSubImage2D(textureID, level_of_detail, xoffset, yoffset, width, height, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateTextureMipmap(textureID);

    // free image
    stbi_image_free(pixels);
} 

OpenglImage::~OpenglImage()
{   
    SPDLOG_DEBUG("destructor");
    glDeleteTextures( num_of_textures, &textureID );
}

void OpenglImage::bind(){
    const GLuint  binding = 1;
    glBindTextures(binding, num_of_textures, &textureID);
}


