#include "Resource/SoundResource.h"

namespace Luden {
	bool SoundResource::Load(std::shared_ptr<IFile> file)
	{
		std::vector<uint8_t> buffer(file->Size());
		file->Read(buffer.data(), buffer.size());
		return m_SoundBuffer.loadFromMemory(buffer.data(), buffer.size());
	}
	void SoundResource::Unload()
	{
	}
	void SoundResource::Init()
	{
	}
	const sf::SoundBuffer& SoundResource::GetSoundBuffer() const
	{
		return m_SoundBuffer;
	}
}
