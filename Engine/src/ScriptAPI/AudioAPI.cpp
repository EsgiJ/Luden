#include "ScriptAPI/AudioAPI.h"
#include "Audio/AudioManager.h"

#include <iostream>

namespace Luden
{
	namespace SoundAPI
	{
		void PlaySound(SoundRef sound, float volume)
		{
			if (!sound)
			{
				std::cerr << "[SoundAPI] Invalid sound reference!" << std::endl;
				return;
			}

			AudioManager::Instance().PlaySound(sound, volume);
		}

		void PlayMusic(std::shared_ptr<Music> music, float volume, bool loop)
		{
			if (!music)
			{
				std::cerr << "[SoundAPI] Invalid music reference!" << std::endl;
				return;
			}

			AudioManager::Instance().PlayMusic(music, volume, loop);
		}

		void StopMusic()
		{
			AudioManager::Instance().StopMusic();
		}

		void PauseMusic()
		{
			AudioManager::Instance().PauseMusic();
		}

		void ResumeMusic()
		{
			AudioManager::Instance().ResumeMusic();
		}

		void SetMusicVolume(float volume)
		{
			AudioManager::Instance().SetMusicVolume(volume);
		}

		void SetSoundVolume(float volume)
		{
			AudioManager::Instance().SetSoundVolume(volume);
		}

		void MuteAll(bool mute)
		{
			AudioManager::Instance().MuteAll(mute);
		}
	}
}