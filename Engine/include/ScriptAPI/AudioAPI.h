#pragma once
#include "EngineAPI.h"
#include "Luden.h"  // SoundRef için

namespace Luden
{
	namespace SoundAPI
	{
		ENGINE_API void PlaySound(SoundRef sound, float volume = 1.0f);
		ENGINE_API void PlayMusic(SoundRef music, float volume = 0.5f, bool loop = true);

		ENGINE_API void StopMusic();
		ENGINE_API void PauseMusic();
		ENGINE_API void ResumeMusic();
		ENGINE_API void SetMusicVolume(float volume);
		ENGINE_API void SetSoundVolume(float volume);
		ENGINE_API void MuteAll(bool mute);
	}
}