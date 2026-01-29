#pragma once
#include "EngineAPI.h"
#include "Resource/Resource.h"

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

#include <filesystem>

namespace Luden
{
    class ENGINE_API Shader : public Resource
    {
    public:
        Shader() = default;
        Shader(const std::string& name) : Resource(name) {}

        bool LoadFromFile(const std::filesystem::path& fragmentPath);
        bool LoadFromFile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


        sf::Shader& GetShader() { return m_Shader; }
        const sf::Shader& GetShader() const { return m_Shader; }

        void SetFloat(const std::string& name, float value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetTexture(const std::string& name, const sf::Texture& texture);

        bool IsLoaded() const { return m_Loaded; }

        void SetFragmentShaderPath(const std::filesystem::path& path) { m_FragmentShaderPath = path; }
        void SetVertexShaderPath(const std::filesystem::path& path) { m_VertexShaderPath = path; }

        const std::filesystem::path& GetFragmentShaderPath() const { return m_FragmentShaderPath; }
        const std::filesystem::path& GetVertexShaderPath() const { return m_VertexShaderPath; }

        bool HasVertexShader() const { return !m_VertexShaderPath.empty(); }

        static ResourceType GetStaticType() { return ResourceType::Shader; }
        virtual ResourceType GetResourceType() const override { return GetStaticType(); }

    private:
        sf::Shader m_Shader;
        bool m_Loaded = false;

        std::filesystem::path m_FragmentShaderPath;
        std::filesystem::path m_VertexShaderPath;
    };
}