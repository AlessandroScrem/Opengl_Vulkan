#pragma once
//common
#include <mytypes.hpp>

//std
#include <string>



class OpenglImage
{
public:
    OpenglImage(const char *path, const std::string &directory){ OpenglImage(std::string(directory + '/' + std::string(path))); }
    OpenglImage(const std::string  &filename);
    ~OpenglImage(){   SPDLOG_DEBUG("destructor"); }
    unsigned int getId(){return id;}
private:
    unsigned int id;
    
};    

