#pragma once

#include <filesystem>

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include <SFML/Audio/Music.hpp>

namespace Luden
{
	class ENGINE_API Music : public Resource
	{
	public:
		Music() = default;
		~Music() override = default;

		bool LoadFromFile();

		float GetDuration() const;

		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		std::filesystem::path& GetFilePath() { return m_FilePath; }
		void SetFilePath(const std::filesystem::path& filePath) { m_FilePath = filePath; }

		const sf::Music& GetMusic() const { return m_Music; }
		sf::Music& GetMusic() { return m_Music; }

		static ResourceType GetStaticType() { return ResourceType::Music; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }
	private:
		std::filesystem::path m_FilePath;
		sf::Music m_Music;
	};
}
