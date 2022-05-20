#include "OpenglShader.hpp"
#include "OpenglImage.hpp"
#include "OpenglUbo.hpp"


OpenglShaderBuilder::OpenglShaderBuilder()
{
    this->Reset();
}

void OpenglShaderBuilder::Reset(){
    this->shader= std::make_unique<OpenglShader>();
}

Builder& OpenglShaderBuilder::type(GLSL::ShaderType id) { 
    this->shader->shaderType = id;        
    return *this;
}

Builder& OpenglShaderBuilder::addUbo(uint32_t binding ) {
    this->shader->addUbo(binding);
    return *this;
}

Builder& OpenglShaderBuilder::addTexture(std::string image, uint32_t binding ) {
    this->shader->addTexture(image, binding);
    return *this;
}

Builder& OpenglShaderBuilder::setPolygonMode(uint32_t mode) {
    switch (mode)
    {
    case 0:
        this->shader->setTopology(GL_TRIANGLES);
        this->shader->setPolygonMode(GL_FILL);
        break;
    case 1:
        this->shader->setTopology(GL_LINES);
        this->shader->setPolygonMode(GL_LINE);
        break;
    
    default:
        break;
    }
    return *this;
}

std::unique_ptr<Shader> OpenglShaderBuilder::build() {
    this->shader->buid();
    std::unique_ptr<Shader> result = std::move(this->shader);
    this->Reset();
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
    glDeleteProgram(shaderProgram);
}

void OpenglShader::buid()
{
    SPDLOG_DEBUG("OpenglShader build"); 
    buildShaders();
}

void  OpenglShader::use(){
    //TODO : set polygonmode & topology
    // glPolygonMode(GL_FRONT_AND_BACK ,polygonMode);

    glUseProgram(shaderProgram);

    if(shaderBindings.image){
        shaderBindings.image->bind();
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

OpenglUbo & OpenglShader::getUbo()
{
    if(!shaderBindings.ubo){
        throw std::runtime_error("failed to get ubo!");        
    }
    return *shaderBindings.ubo;
}

void  OpenglShader::addTexture(std::string imagepath, uint32_t binding)
{
    shaderBindings.image = std::make_unique<OpenglImage>(imagepath);
    shaderBindings.imageBindig = binding;
}

void  OpenglShader::addUbo(uint32_t binding)
{
    shaderBindings.ubo = std::make_unique<OpenglUbo>();
    shaderBindings.uboBindig = binding;
}

void  OpenglShader::addConstant(uint32_t binding)
{

}
void  OpenglShader::setPolygonMode(GLenum mode)
{
    polygonMode = mode;
}
void  OpenglShader::setTopology(GLenum  mode)
{
    topology = mode;
}

