#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"
#include "Core/Utils.h"

namespace VolcaniCore {

Shader::ShaderFile FindShaderFile(const std::string& path);
uint32_t CreateProgram(const std::vector<Shader::ShaderFile>& shader_files);

Shader::Shader(const std::vector<std::string>& paths)
{
    std::vector<ShaderFile> files;
    files.reserve(paths.size());

    for(const auto& path : paths)
    {
        ShaderFile file = FindShaderFile(path);
        files.push_back(file);
    }

    m_ProgramID = CreateProgram(files);
}

Shader::Shader(const std::initializer_list<ShaderFile>& files)
{
    m_ProgramID = CreateProgram(files);
}

Shader::Shader(const std::string& folder_path, const std::string& name)
{
    // TODO: Find all the different kinds shaders in the folder that have the same name
}

Shader::~Shader() { glDeleteProgram(m_ProgramID); }

void Shader::Bind() const { glUseProgram(m_ProgramID); }
void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetInt(const std::string& name, int _int)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(location, _int);
}

void Shader::SetFloat(const std::string& name, float _float)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1f(location, _float);
}

void Shader::SetTexture(const std::string& name, Texture2D* texture)
{
    VOLCANICORE_ASSERT(texture->GetSlot() >= 0);
    SetInt(name, texture->GetSlot());
}

void Shader::SetVec2(const std::string& name, const glm::vec2& vec)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform2f(location, vec.x, vec.y);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& vec)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform4f(location, vec.r, vec.g, vec.b, vec.a);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& matrix)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t GetShaderType(ShaderType type)
{
    switch(type)
    {
        case ShaderType::Vertex:   return GL_VERTEX_SHADER;
        case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Compute:  return GL_COMPUTE_SHADER;
    }

    return 0;
}

bool StringContains(const std::string& str, const std::string& sub_str) { return str.find(sub_str) != std::string::npos; }

Shader::ShaderFile FindShaderFile(const std::string& path)
{
    std::size_t dot = path.find_first_of('.');
    if(dot == std::string::npos)
        VOLCANICORE_ASSERT_ARGS(false, "%s is an incorrectly formatted file name. Accepted formats: example.glsl.vert, example.vert.glsl, example.vert", path.c_str());

    std::string sub_str = path.substr(dot);
    if(StringContains(sub_str, "vert") || StringContains(sub_str, "vs")) return Shader::ShaderFile{ path, ShaderType::Vertex };
    if(StringContains(sub_str, "geom") || StringContains(sub_str, "gs")) return Shader::ShaderFile{ path, ShaderType::Geometry };
    if(StringContains(sub_str, "frag") || StringContains(sub_str, "fs")) return Shader::ShaderFile{ path, ShaderType::Fragment };
    if(StringContains(sub_str, "comp") || StringContains(sub_str, "compute")) return Shader::ShaderFile{ path, ShaderType::Compute };

    VOLCANICORE_ASSERT_ARGS(false, "File %s is of unknown shader type", path.c_str());
    return Shader::ShaderFile{ "", ShaderType::Unknown };
}

uint32_t CreateShader(const Shader::ShaderFile& file)
{
    uint32_t type = GetShaderType(file.Type);
    uint32_t shader_id = glCreateShader(type);

    std::string source = Utils::ReadFile(file.Path);
    const char* address = source.c_str();
    glShaderSource(shader_id, 1, &address, nullptr);
    glCompileShader(shader_id);

    int result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shader_id, length, &length, message);

        VOLCANICORE_ASSERT_ARGS(false, "A compile error was detected for shader file at %s:\n%s", file.Path.c_str(), message);
    }

    return shader_id;
}

uint32_t CreateProgram(const std::vector<Shader::ShaderFile>& shader_files)
{
    uint32_t program_id = glCreateProgram();
    std::vector<uint32_t> shader_ids(shader_files.size());

    for(const auto& shader : shader_files)
    {
        uint32_t shader_id = CreateShader(shader);
        glAttachShader(program_id, shader_id);
        shader_ids.push_back(shader_id);
    }

    glLinkProgram(program_id);

    int result;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);

    if(result == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program_id, length, &length, message);

        VOLCANICORE_ASSERT_ARGS(false, "An linking error has occured: \n%s", message);

        glDeleteProgram(program_id);
        return 0;
    }

    for(auto& shader_id : shader_ids)
    {
        glDetachShader(program_id, shader_id);
        glDeleteShader(shader_id);
    }

    return program_id;
}

}