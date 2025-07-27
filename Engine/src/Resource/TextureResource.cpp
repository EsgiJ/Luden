#include "Resource/TextureResource.h"

namespace Luden {
	bool TextureResource::Load(std::shared_ptr<IFile> file)
	{
		std::vector<uint8_t> buffer(file->Size());
		file->Read(buffer.data(), buffer.size());
		return m_Texture.loadFromMemory(buffer.data(), buffer.size());
	}
	void TextureResource::Unload()
	{
	}
	void TextureResource::Init()
	{
	}
	const sf::Texture& TextureResource::GetTexture() const
	{
		return m_Texture;
	}
}
