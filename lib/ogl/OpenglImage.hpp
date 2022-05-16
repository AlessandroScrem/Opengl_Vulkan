#pragma once
//common
#include <mytypes.hpp>

//std
#include <string>



class OpenglImage
{
public:
    OpenglImage(const std::string  &filename = "data/textures/viking_room.png");
    ~OpenglImage(){   SPDLOG_DEBUG("destructor"); }

    void bind();
private:
    unsigned int id;
    
};    


