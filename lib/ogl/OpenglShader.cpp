#include "OpenglShader.hpp"


void OpenglShader::buildShaders()
{
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    auto glsl_vert = GLSL::readFile(GLSL::getname(shaderType) + ".vert");
    auto glsl_frag = GLSL::readFile(GLSL::getname(shaderType) + ".frag");

    compile(vert_shader, glsl_vert, GL_VERTEX_SHADER);
    compile(frag_shader, glsl_frag, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vert_shader );
    glAttachShader(shaderProgram, frag_shader );
 
    link();

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

void OpenglShader::link(){
    
    // link shaders
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
    }
}

void OpenglShader::compile(GLuint shader, std::vector<char> &glsl, GLenum  kind)
{ 
    auto source = std::string(glsl.begin(), glsl.end());
    const char * ShaderCode = source.c_str();

    // build and compile our shader program
    glShaderSource(shader, 1, &ShaderCode, NULL);
    glCompileShader(shader);
   
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::string shadername;
        switch (kind)
        {
        case GL_VERTEX_SHADER:
            shadername = "GL_VERTEX_SHADER";
            break;
        
        case GL_FRAGMENT_SHADER:
            shadername = "GL_FRAGMENT_SHADER";
            break;
        
        default:
            shadername = "unknown";
            break;
        }     
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        spdlog::error("ERROR::SHADER::{}::COMPILATION_FAILED\n {}", shadername, infoLog);
    } 
}