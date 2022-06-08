#include "OpenglShader.hpp"
#include "OpenglImage.hpp"
#include "OpenglUbo.hpp"



ShaderBuilder& OpenglShaderBuilder::Reset(){
    this->shader = std::make_unique<OpenglShader>();
    return *this;
}

ShaderBuilder& OpenglShaderBuilder::type(GLSL::ShaderType id) { 
    this->shader->shaderType = id;        
    return *this;
}


ShaderBuilder& OpenglShaderBuilder::addTexture(std::string imagepath, uint32_t binding ) 
{
    auto image =  std::make_unique<OpenglImage>(imagepath);
    this->shader->shaderBindings.image.emplace(binding, std::move(image));
    return *this;
}

ShaderBuilder& OpenglShaderBuilder::setPolygonMode(uint32_t mode) {
    switch (mode)
    {
    case 0:
        this->shader->topology = GL_TRIANGLES;
        this->shader->polygonMode = GL_FILL;
        break;
    case 1:
        this->shader->topology = GL_LINES;
        this->shader->polygonMode = GL_LINE;
        break;
    
    default:
        break;
    }
    return *this;
}

std::unique_ptr<Shader> OpenglShaderBuilder::build() {
    this->shader->buid();
    std::unique_ptr<Shader> result = std::move(this->shader);
    return result;
}



OpenglShader::OpenglShader(GLSL::ShaderType type /* = GLSL::PHONG */) 
    : shaderType{type}
{
    SPDLOG_TRACE("OpenglShader constructor"); 
}

OpenglShader::~OpenglShader()
{
    SPDLOG_DEBUG("OpenglShader destructor"); 

    if(prepared){
        glDeleteProgram(shaderProgram);
    }
}

void OpenglShader::buid()
{
    SPDLOG_DEBUG("OpenglShader build");
    createGlobalUbo(); 
    buildShaders();
    prepared = true;
}

void  OpenglShader::bind(){
    //TODO : set polygonmode & topology
    // glPolygonMode(GL_FRONT_AND_BACK ,polygonMode);

    glUseProgram(shaderProgram);

    for(auto& shaderBinding : shaderBindings.image){
        shaderBinding.second->bind();
    }

    for(auto& shaderBinding : shaderBindings.ubo){
        shaderBinding.second->bind();
    }

}

void OpenglShader::buildShaders()
{
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    auto glsl_vert = GLSL::readFile(GLSL::getPath(shaderType) + ".vert");
    auto glsl_frag = GLSL::readFile(GLSL::getPath(shaderType) + ".frag");

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

void OpenglShader::updateUbo(UniformBufferObject & mvp)
{
    auto& ubo = *(shaderBindings.ubo.at(globalUboBinding));

    ubo.model = mvp.model;
    ubo.view  = mvp.view;
    ubo.proj  = mvp.proj;
    ubo.viewPos = mvp.viewPos;
    
}

void  OpenglShader::createGlobalUbo()
{
    auto ubo = std::make_unique<OpenglUbo>(); 
    shaderBindings.ubo.emplace(globalUboBinding, std::move(ubo) );
}

void  OpenglShader::addConstant(uint32_t binding)
{

}


