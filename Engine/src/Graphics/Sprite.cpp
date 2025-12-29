#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Project/Project.h"
#include "Resource/ResourceManager.h"

namespace Luden
{
	sf::Vector2u Sprite::GetSize()
	{
		if (UsesFullTexture())
		{
			auto texture = ResourceManager::GetResource<Texture>(m_TextureHandle);
			return texture->GetTexture().getSize();
		}
		else
		{
			return { static_cast<unsigned int>(m_TextureRect.size.x), static_cast<unsigned int>(m_TextureRect.size.y) };
		}
	}

	std::shared_ptr<Texture> Sprite::GetTexture()
	{
		if (m_TextureHandle == 0)
			return nullptr;

		auto texture = ResourceManager::GetResource<Texture>(m_TextureHandle);
		return texture;
	}
}