#include "OpenglShader.hpp"
#include "OpenglImage.hpp"
#include "OpenglUbo.hpp"

std::string getShaderInfoLog(GLuint shader) {
    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

    std::string log;
    if (logLen > 0) {
        log.resize(logLen, ' ');
        GLsizei written;
        glGetShaderInfoLog(shader, logLen, &written, &log[0]);
    }

    return log;
}

std::string getProgramInfoLog(GLuint program) {
    GLint logLen;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLen );

    std::string log;
    if (logLen > 0) {
        log.resize(logLen, ' ');
        GLsizei written;
        glGetProgramInfoLog(program, logLen, &written, &log[0]);
    }
    return log;
}

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

ShaderBuilder& OpenglShaderBuilder::setPolygonMode(GLSL::PolygonMode mode) {
    switch (mode)
    {
    case GLSL::TRIANGLES:
        this->shader->topology = GL_TRIANGLES;
        this->shader->polygonMode = GL_FILL;
        break;
    case GLSL::LINES:
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
    buildShaders();
    prepared = true;
}

void  OpenglShader::bind(GLenum mode){
    //TODO : set polygonmode & topology
    
    glPolygonMode(GL_FRONT_AND_BACK ,mode);

    glUseProgram(shaderProgram);

    for(auto& shaderBinding : shaderBindings.image){
        shaderBinding.second->bind();
    }

}

void OpenglShader::buildShaders()
{
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint status;

    auto glsl_vert = GLSL::readFile(GLSL::getPath(shaderType) + ".vert.spv");
    auto glsl_frag = GLSL::readFile(GLSL::getPath(shaderType) + ".frag.spv");

    // if (glsl)
    // compile(vert_shader, glsl_vert, GL_VERTEX_SHADER);
    // compile(frag_shader, glsl_frag, GL_FRAGMENT_SHADER);

    glShaderBinary(1, &vert_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, glsl_vert.data(), static_cast<GLsizei>(glsl_vert.size()));
    glShaderBinary(1, &frag_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, glsl_frag.data(), static_cast<GLsizei>(glsl_frag.size()));

    glSpecializeShader( vert_shader, "main", 0, nullptr, nullptr);
    glSpecializeShader( frag_shader, "main", 0, nullptr, nullptr);

    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &status);
    if( GL_FALSE == status ) {
        spdlog::error("Failed to load vertex shader (SPIR-V) {}", getShaderInfoLog(vert_shader) );
    }

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);
    if( GL_FALSE == status ) {
        spdlog::error("Failed to load fragment shader (SPIR-V) {}", getShaderInfoLog(frag_shader) );
    }

    // Create the program object
    shaderProgram = glCreateProgram();
    if (0 == shaderProgram) {
        spdlog::error("Error creating program object.");
    }

    glAttachShader(shaderProgram, vert_shader);
    glAttachShader(shaderProgram, frag_shader);
    
    link();

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    // glUseProgram(shaderProgram);
}

void OpenglShader::link()
{
    GLint status;

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        spdlog::error("Failed to link SPIR-V program {}", getProgramInfoLog(shaderProgram));
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



