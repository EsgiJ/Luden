#include "Resource/FontResource.h"

namespace Luden {
	bool FontResource::Load(std::shared_ptr<IFile> file)
	{
		std::vector<uint8_t> buffer(file->Size());
		file->Read(buffer.data(), buffer.size());
		return m_Font.openFromMemory(buffer.data(), buffer.size());
	}
	void FontResource::Unload()
	{
	}
	void FontResource::Init()
	{
	}
	const sf::Font& FontResource::GetFont() const
	{
		return m_Font;
	}
}
