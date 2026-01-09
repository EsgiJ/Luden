#include "Audio/AudioManager.h"
#include "Audio/Sound.h"
#include "Audio/Music.h"
#include "Core/EngineContext.h"

#include <algorithm>

#include "SFML/Audio/SoundStream.hpp"

namespace Luden
{
	void AudioManager::PlaySound(std::shared_ptr<Sound> sound, float volume)
	{
		if (!sound)
			return;
		sf::Sound sfSound(sound->GetSoundBuffer());

		float finalVolume = volume * m_SoundVolume * m_MasterVolume;
		if (m_Muted)
			finalVolume = 0.0f;

		sfSound.setVolume(finalVolume * 100.0f);
		sfSound.play();

		m_ActiveSounds.push_back(std::move(sfSound));
	}

	void AudioManager::StopAllSounds()
	{
		for (auto& instance : m_ActiveSounds)
		{
			instance.stop();
		}
		m_ActiveSounds.clear();
	}

	void AudioManager::PlayMusic(std::shared_ptr<Music> music, float volume, bool loop)
	{
		if (!music)
			return;

		StopMusic();

		m_CurrentMusic = music;

		auto& sfMusic = m_CurrentMusic->GetMusic();
		sfMusic.setLooping(loop);

		float finalVolume = volume * m_MusicVolume * m_MasterVolume;
		if (m_Muted)
			finalVolume = 0.0f;

		sfMusic.setVolume(finalVolume * 100.0f);
		sfMusic.play();
	}

	void AudioManager::StopMusic()
	{
		if (m_CurrentMusic)
		{
			m_CurrentMusic->GetMusic().stop();
			m_CurrentMusic.reset();
		}
	}

	void AudioManager::PauseMusic()
	{
		if (m_CurrentMusic)
		{
			m_CurrentMusic->GetMusic().pause();
		}
	}

	void AudioManager::ResumeMusic()
	{
		if (m_CurrentMusic)
		{
			m_CurrentMusic->GetMusic().play();
		}
	}

	bool AudioManager::IsMusicPlaying() const
	{
		if (m_CurrentMusic)
		{
			return m_CurrentMusic->GetMusic().getStatus() == sf::SoundSource::Status::Playing;
		}
		return false;
	}

	void AudioManager::SetMasterVolume(float volume)
	{
		m_MasterVolume = std::clamp(volume, 0.0f, 1.0f);

		for (auto& instance : m_ActiveSounds)
		{
			float finalVolume = m_SoundVolume * m_MasterVolume;
			if (m_Muted)
				finalVolume = 0.0f;
			instance.setVolume(finalVolume * 100.0f);
		}

		if (m_CurrentMusic)
		{
			float finalVolume = m_MusicVolume * m_MasterVolume;
			if (m_Muted)
				finalVolume = 0.0f;
			m_CurrentMusic->GetMusic().setVolume(finalVolume * 100.0f);
		}
	}

	void AudioManager::SetSoundVolume(float volume)
	{
		m_SoundVolume = std::clamp(volume, 0.0f, 1.0f);

		for (auto& instance : m_ActiveSounds)
		{
			float finalVolume = m_SoundVolume * m_MasterVolume;
			if (m_Muted)
				finalVolume = 0.0f;
			instance.setVolume(finalVolume * 100.0f);
		}
	}

	void AudioManager::SetMusicVolume(float volume)
	{
		m_MusicVolume = std::clamp(volume, 0.0f, 1.0f);

		if (m_CurrentMusic)
		{
			float finalVolume = m_MusicVolume * m_MasterVolume;
			if (m_Muted)
				finalVolume = 0.0f;
			m_CurrentMusic->GetMusic().setVolume(finalVolume * 100.0f);
		}
	}

	void AudioManager::MuteAll(bool mute)
	{
		m_Muted = mute;

		float volume = mute ? 0.0f : 1.0f;

		for (auto& instance : m_ActiveSounds)
		{
			float finalVolume = m_SoundVolume * m_MasterVolume * volume;
			instance.setVolume(finalVolume * 100.0f);
		}

		if (m_CurrentMusic)
		{
			float finalVolume = m_MusicVolume * m_MasterVolume * volume;
			m_CurrentMusic->GetMusic().setVolume(finalVolume * 100.0f);
		}
	}

	void AudioManager::Update()
	{
		m_ActiveSounds.erase(
			std::remove_if(m_ActiveSounds.begin(), m_ActiveSounds.end(),
				[](const sf::Sound& instance) {
					return instance.getStatus() == sf::SoundSource::Status::Stopped;
				}),
			m_ActiveSounds.end()
		);
	}

	void AudioManager::Shutdown()
	{
		StopAllSounds();
		StopMusic();
	}
}