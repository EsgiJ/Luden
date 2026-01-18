#include "Audio/Sound.h"

namespace Luden
{
	bool Sound::LoadFromFile()
	{
		if (!m_SoundBuffer.loadFromFile(m_FilePath.string()))
			return false;

		m_Sound.emplace(m_SoundBuffer);

		return true;
	}

	float Sound::GetDuration() const
	{
		return m_SoundBuffer.getDuration().asSeconds();
	}
}

