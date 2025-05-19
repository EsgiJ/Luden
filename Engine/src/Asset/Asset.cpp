#include "Asset/Asset.h"

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
		m_Textures.emplace(name, texture);
	}

	void Assets::AddFont(const std::string& name, const std::string& path) {
		sf::Font font;
		if (!font.openFromFile(path)) 
		{
			std::cerr << "Failed to load font: " << path << "\n";
			exit(-1);
		}
		m_Fonts.emplace(name, font);
	}

	void Assets::AddSound(const std::string& name, const std::string& path)
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(path)) {
			std::cerr << "Failed to load sound: " << path << "\n";
			exit(-1);
		}
		m_SoundBuffers.emplace(name, buffer);
		m_Sounds.emplace(name, sf::Sound(m_SoundBuffers.at(name))); 
	}

	void Assets::AddAnimation(const std::string& name, const Graphics::Animation& animation)
	{
		m_Animations.emplace(name, animation); 
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

	const std::map<std::string, Graphics::Animation>& Assets::GetAnimations() const {
		return m_Animations;
	}

	const std::map<std::string, sf::Texture>& Assets::GetTextures() const {
		return m_Textures;
	}

	const std::map<std::string, sf::Font>& Assets::GetFonts() const {
		return m_Fonts;
	}

	const std::map<std::string, sf::Sound>& Assets::GetSounds() const {
		return m_Sounds;
	}

	const std::map<std::string, sf::SoundBuffer>& Assets::GetSoundBuffers() const {
		return m_SoundBuffers;
	}

} 
