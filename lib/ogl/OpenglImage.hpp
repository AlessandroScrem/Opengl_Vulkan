#pragma once

//std
#include <string>



class OpenglImage
{
public:
    OpenglImage(const char *path, const std::string &directory){ OpenglImage(std::string(directory + '/' + std::string(path))); }
    OpenglImage(const std::string  &filename);

    unsigned int getId(){return id;}
private:
    unsigned int id;
    
};    


