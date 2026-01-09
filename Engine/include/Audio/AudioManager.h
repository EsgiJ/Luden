#pragma once
#include "EngineAPI.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include <unordered_map>

namespace Luden
{
	class Sound;
	class Music;

	class ENGINE_API AudioManager
	{
	public:
		static AudioManager& Instance()
		{
			static AudioManager s_Instance;
			return s_Instance;
		}

		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;

		void PlaySound(std::shared_ptr<Sound> sound, float volume = 1.0f);
		void StopAllSounds();

		void PlayMusic(std::shared_ptr<Music> music, float volume = 0.5f, bool loop = true);
		void StopMusic();
		void PauseMusic();
		void ResumeMusic();
		bool IsMusicPlaying() const;

		void SetMasterVolume(float volume);
		void SetSoundVolume(float volume);
		void SetMusicVolume(float volume);
		void MuteAll(bool mute);

		void Update();
		void Shutdown();

	private:
		AudioManager() = default;

		std::vector<sf::Sound> m_ActiveSounds;

		std::shared_ptr<Music> m_CurrentMusic;

		float m_MasterVolume = 1.0f;
		float m_SoundVolume = 1.0f;
		float m_MusicVolume = 0.5f;
		bool m_Muted = false;
	};
}