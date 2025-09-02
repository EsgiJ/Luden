#pragma once

#include <filesystem>

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include <SFML/Audio/SoundBuffer.hpp>

namespace Luden
{
	class ENGINE_API SoundBuffer : public Resource
	{
	public:
		SoundBuffer() = default;
		~SoundBuffer() override = default;

		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		std::filesystem::path& GetFilePath() { return m_FilePath; }
		void SetFilePath(const std::filesystem::path& filePath) { m_FilePath = filePath; }

		const sf::SoundBuffer& GetBuffer() const { return m_Buffer; }
		sf::SoundBuffer& GetBuffer() { return m_Buffer; }
		void SetBuffer(const sf::SoundBuffer soundBuffer) { m_Buffer = soundBuffer; }

		static ResourceType GetStaticType() { return ResourceType::Audio; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }
	private:
		std::filesystem::path m_FilePath;
		sf::SoundBuffer m_Buffer;
	};
}
