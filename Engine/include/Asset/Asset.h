#pragma once

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "EngineAPI.h"
#include "Graphics/Animation.h"

namespace Luden {

	class ENGINE_API Assets {
		std::map<std::string, sf::Texture>     m_Textures;
		std::map<std::string, sf::Font>        m_Fonts;
		std::map<std::string, sf::SoundBuffer> m_SoundBuffers;
		std::map<std::string, sf::Sound>       m_Sounds;
		std::map<std::string, Animation>       m_Animations;

	public:
		void LoadFromFile(const std::string& path);

		void AddTexture(const std::string& name, const std::string& path);
		void AddFont(const std::string& name, const std::string& path);
		void AddSound(const std::string& name, const std::string& path);
		void AddAnimation(const std::string& name, const Animation& animation);

		sf::Texture& GetTexture(const std::string& name);
		sf::Font& GetFont(const std::string& name);
		sf::Sound& GetSound(const std::string& name);
		Animation& GetAnimation(const std::string& name);
	};

}
