#pragma once

#include <filesystem>

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include <SFML/Audio/SoundBuffer.hpp>

namespace Luden
{
	class ENGINE_API Sound : public Resource
	{
	public:
		Sound() = default;
		~Sound() override = default;

		bool LoadFromFile();

		float GetDuration() const;

		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		std::filesystem::path& GetFilePath() { return m_FilePath; }
		void SetFilePath(const std::filesystem::path& filePath) { m_FilePath = filePath; }

		const sf::SoundBuffer& GetSoundBuffer() const { return m_SoundBuffer; }
		sf::SoundBuffer& GetSoundBuffer() { return m_SoundBuffer; }
		void SetSoundBuffer(const sf::SoundBuffer soundBuffer) { m_SoundBuffer = soundBuffer; }

		static ResourceType GetStaticType() { return ResourceType::Sound; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }
	private:
		std::filesystem::path m_FilePath;
		sf::SoundBuffer m_SoundBuffer;
	};
}
