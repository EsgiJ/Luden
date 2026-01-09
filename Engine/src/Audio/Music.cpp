#include "Audio/Music.h"

bool Luden::Music::LoadFromFile()
{
	if (!m_Music.openFromFile(m_FilePath.string()))
		return false;

	return true;
}

float Luden::Music::GetDuration() const
{
	return m_Music.getDuration().asSeconds();
}
