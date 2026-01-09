#include "Audio/Sound.h"

bool Luden::Sound::LoadFromFile()
{
	if (!m_SoundBuffer.loadFromFile(m_FilePath.string()))
		return false;

	return true;
}

float Luden::Sound::GetDuration() const
{
	return m_SoundBuffer.getDuration().asSeconds();
}
