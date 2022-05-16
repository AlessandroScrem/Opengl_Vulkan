#pragma once
#include <vector>
#include <string>
#include <fstream>

namespace GLSL {

enum ShaderType{
    AXIS,
    PHONG,
    TEXTURE,
    NORMALMAP
};

constexpr const char * prefixpath = "data/shaders/";
static std::string getName(ShaderType type);
static std::string getPath(ShaderType type) { return prefixpath + getName(type); }

std::string getName(ShaderType type){
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
    return name;
}

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
} // GLSL namespace
