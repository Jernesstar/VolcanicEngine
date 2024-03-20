#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

enum class ShaderType { Vertex, Geometry, Fragment, Compute, Unknown };

class Shader {
public:
    struct ShaderFile {
        const std::string Path;
        const ShaderType Type;
    };

public:
    Shader(const std::vector<std::string>& paths);
    Shader(const std::initializer_list<ShaderFile>& files);
    Shader(const std::string& folder_path, const std::string& name);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetInt(const std::string& name, int _int);
    void SetFloat(const std::string& name, float _float);
    void SetTexture(const std::string& name, Texture2D* texture);

    void SetVec2(const std::string& name, const glm::vec2& vec);
    void SetVec3(const std::string& name, const glm::vec3& vec);
    void SetVec4(const std::string& name, const glm::vec4& vec);

    void SetMat2(const std::string& name, const glm::mat2& mat);
    void SetMat3(const std::string& name, const glm::mat3& mat);
    void SetMat4(const std::string& name, const glm::mat4& mat);

private:
    uint32_t m_ProgramID;
};

}