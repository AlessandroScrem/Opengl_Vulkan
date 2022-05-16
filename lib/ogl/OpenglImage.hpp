#pragma once
//std
#include <string>

class OpenglImage
{
public:
    OpenglImage(const std::string  &filename = "data/textures/viking_room.png");
    ~OpenglImage();

    void bind();
private:
    unsigned int id;
    
};    


