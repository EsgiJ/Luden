#include "Graphics/Shader.h"

#include <iostream>

namespace Luden
{
    bool Shader::LoadFromFile(const std::filesystem::path& fragmentPath)
    {
        if (!m_Shader.loadFromFile(fragmentPath.string(), sf::Shader::Type::Fragment))
            return false;

        m_Loaded = true;

        return true;
    }

    bool Shader::LoadFromFile(const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath)
    {
        if (!m_Shader.loadFromFile(vertexPath.string(), fragmentPath.string()))
            return false;

        m_Loaded = true;

        return true;
    }

    void Shader::SetFloat(const std::string& name, float value)
    {
        m_Shader.setUniform(name, value);
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value)
    {
        m_Shader.setUniform(name, sf::Glsl::Vec2(value.x, value.y));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value)
    {
        m_Shader.setUniform(name, sf::Glsl::Vec3(value.x, value.y, value.z));
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value)
    {
        m_Shader.setUniform(name, sf::Glsl::Vec4(value.x, value.y, value.z, value.w));
    }

    void Shader::SetTexture(const std::string& name, const sf::Texture& texture)
    {
        m_Shader.setUniform(name, texture);
    }
}