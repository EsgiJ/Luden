#include "Asset/Asset.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Luden {

	void Assets::LoadFromFile(const std::string& path) {
		std::ifstream file(path);
		if (!file) {
			std::cerr << "Failed to open assets file: " << path << "\n";
			return;
		}

		std::string type;
		while (file >> type) {
			if (type == "Texture") {
				std::string name, texturePath;
				file >> name >> texturePath;
				AddTexture(name, texturePath);
			}
			else if (type == "Font") {
				std::string name, fontPath;
				file >> name >> fontPath;
				AddFont(name, fontPath);
			}
			else if (type == "Sound") {
				std::string name, soundPath;
				file >> name >> soundPath;
				AddSound(name, soundPath);
			}
			else if (type == "Animation") {
				std::string name, textureName;
				int frameCount, speed, x, y, w, h;
				file >> name >> frameCount >> speed >> textureName >> x >> y >> w >> h;
				Animation anim(GetTexture(textureName), frameCount, speed, { x, y, w, h });
				AddAnimation(name, anim);
			}
		}
	}

	void Assets::AddTexture(const std::string& name, const std::string& path) {
		sf::Texture tex;
		if (!tex.loadFromFile(path)) {
			std::cerr << "Failed to load texture: " << path << "\n";
			return;
		}
		m_Textures[name] = std::move(tex);
	}

	void Assets::AddFont(const std::string& name, const std::string& path) {
		sf::Font font;
		if (!font.loadFromFile(path)) {
			std::cerr << "Failed to load font: " << path << "\n";
			return;
		}
		m_Fonts[name] = std::move(font);
	}

	void Assets::AddSound(const std::string& name, const std::string& path) {
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(path)) {
			std::cerr << "Failed to load sound: " << path << "\n";
			return;
		}
		m_SoundBuffers[name] = buffer;

		sf::Sound sound;
		sound.setBuffer(m_SoundBuffers[name]);
		m_Sounds[name] = std::move(sound);
	}

	void Assets::AddAnimation(const std::string& name, const Animation& animation) {
		m_Animations[name] = animation;
	}

	sf::Texture& Assets::GetTexture(const std::string& name) {
		return m_Textures.at(name);
	}

	sf::Font& Assets::GetFont(const std::string& name) {
		return m_Fonts.at(name);
	}

	sf::Sound& Assets::GetSound(const std::string& name) {
		return m_Sounds.at(name);
	}

	Animation& Assets::GetAnimation(const std::string& name) {
		return m_Animations.at(name);
	}

}
