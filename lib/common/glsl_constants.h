#pragma once
#include <vector>
#include <string>
#include <fstream>

namespace GLSL {

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
enum ShaderType{
    AXIS,
    PHONG,
    TEXTURE,
    NORMALMAP
};

static std::string getname(ShaderType type){
    std::string prefixpath = "data/shaders/";
    std::string name{};
        switch (type)
        {
        case AXIS:
            name =  "axis";
            break;
        case PHONG:
            name =  "phong";
            break;
        case TEXTURE:
            name = "texture";
            break;
        case NORMALMAP:
            name = "normalmap";
            break;       
        default:
            break;
        }
    return prefixpath + name;
}


} // GLSL namespace
