#pragma once
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//std
#include <string>

class OpenglImage
{
public:
    OpenglImage(const std::string  &filename = "data/textures/viking_room.png");
    ~OpenglImage();

    void bind();
private:
    GLuint textureID;

    const int num_of_textures = 1;
    
};    


