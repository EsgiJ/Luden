#pragma once

#include <map>
#include <string>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>

#include "EngineAPI.h"
#include "Graphics/Animation.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden {

	class ENGINE_API Assets 
	{
		RTTR_REGISTRATION_FRIEND

		std::map<std::string, sf::Texture>	m_Textures;
		std::map<std::string, sf::Font>	m_Fonts;
		std::map<std::string, sf::SoundBuffer> m_SoundBuffers;
		std::map<std::string, sf::Sound> m_Sounds;
		std::map<std::string, Graphics::Animation> m_Animations;

	public:
		Assets() = default;

		~Assets() = default;

		void LoadFromFile(const std::string& path);

		void AddTexture(const std::string& name, const std::string& path);
		void AddFont(const std::string& name, const std::string& path);
		void AddSound(const std::string& name, const std::string& path);
		void AddAnimation(const std::string& name, const Graphics::Animation& animation);

		sf::Texture& GetTexture(const std::string& name);
		sf::Font& GetFont(const std::string& name);
		sf::Sound& GetSound(const std::string& name);
		Graphics::Animation& GetAnimation(const std::string& name);

		const std::map<std::string, Graphics::Animation>& GetAnimations() const;
		const std::map<std::string, sf::Texture>& GetTextures() const;
		const std::map<std::string, sf::Font>& GetFonts() const;
		const std::map<std::string, sf::Sound>& GetSounds() const;
		const std::map<std::string, sf::SoundBuffer>& GetSoundBuffers() const;

	};

}
