#include "Asset/Asset.h"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Graphics.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

namespace Luden {

	void Assets::LoadFromFile(const std::string& path) {
		std::ifstream file(path);
		if (!file) {
			std::cerr << "Could not load asset file: " << path << "\n";
			exit(-1);
		}

		std::string assetType;
		while (file >> assetType) {
			if (assetType == "Texture") {
				std::string name, filePath;
				file >> name >> filePath;
				AddTexture(name, filePath);
			}
			else if (assetType == "Font") {
				std::string name, filePath;
				file >> name >> filePath;
				AddFont(name, filePath);
			}
			else if (assetType == "Sound") {
				std::string name, filePath;
				file >> name >> filePath;
				AddSound(name, filePath);
			}
			else if (assetType == "Animation") {
				std::string name, textureName;
				int frameCount, speed;
				file >> name >> textureName >> frameCount >> speed;

				auto& texture = GetTexture(textureName);
				AddAnimation(name, Graphics::Animation(name, texture, frameCount, speed));
			}
			else {
				std::cerr << "Unknown asset type: " << assetType << "\n";
				exit(-1);
			}
		}
	}

	void Assets::AddTexture(const std::string& name, const std::string& path) {
		sf::Texture texture;
		if (!texture.loadFromFile(path)) {
			std::cerr << "Failed to load texture: " << path << "\n";
			exit(-1);
		}
		m_Textures[name] = std::move(texture);
	}

	void Assets::AddFont(const std::string& name, const std::string& path) {
		sf::Font font;
		if (!font.openFromFile(path)) 
		{
			std::cerr << "Failed to load font: " << path << "\n";
			exit(-1);
		}
		m_Fonts[name] = std::move(font);
	}

	void Assets::AddSound(const std::string& name, const std::string& path) {
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(path)) {
			std::cerr << "Failed to load sound: " << path << "\n";
			exit(-1);
		}
		m_SoundBuffers[name] = std::move(buffer);
		m_Sounds[name] = sf::Sound(m_SoundBuffers.at(name));
	}

	void Assets::AddAnimation(const std::string& name, const Graphics::Animation& animation) {
		m_Animations[name] = animation;
	}

	sf::Texture& Assets::GetTexture(const std::string& name) {
		assert(m_Textures.find(name) != m_Textures.end() && "Texture not found!");
		return m_Textures.at(name);
	}

	sf::Font& Assets::GetFont(const std::string& name) {
		assert(m_Fonts.find(name) != m_Fonts.end() && "Font not found!");
		return m_Fonts.at(name);
	}

	sf::Sound& Assets::GetSound(const std::string& name) {
		assert(m_Sounds.find(name) != m_Sounds.end() && "Sound not found!");
		return m_Sounds.at(name);
	}

	Graphics::Animation& Assets::GetAnimation(const std::string& name) {
		assert(m_Animations.find(name) != m_Animations.end() && "Animation not found!");
		return m_Animations.at(name);
	}

} 
