#include "Graphics/Animation.h"
#include "Project/Project.h"

#include <cmath>
#include <utility>


namespace Luden::Graphics
{
	Animation::Animation(const std::string& name, const ResourceHandle& textureHandle)
		: m_Sprite(MakeSpriteFromHandle(textureHandle))
		, m_Name(name)
		, m_TextureHandle(textureHandle)
		, m_FrameCount(1)
	{
		auto texture = std::static_pointer_cast<Texture>(Project::GetResourceManager()->GetResource(textureHandle));
		m_Sprite.setTexture(texture->GetTexture());
	}

	Animation::Animation(const std::string& name, const ResourceHandle& textureHandle, size_t frameCount)
		: m_Sprite(MakeSpriteFromHandle(textureHandle))
		, m_Name(name)
		, m_TextureHandle(textureHandle)
		, m_FrameCount(frameCount)
	{
		auto texture = std::static_pointer_cast<Texture>(Project::GetResourceManager()->GetResource(textureHandle));
		m_Size = Math::Vec2(
			static_cast<float>(texture->GetTexture().getSize().x) / static_cast<float>(frameCount),
			static_cast<float>(texture->GetTexture().getSize().y)
		);

		m_Sprite.setTexture(texture->GetTexture());
		m_Sprite.setOrigin(sf::Vector2f(m_Size.x / 2.0f, m_Size.y / 2.0f));

		m_Sprite.setTextureRect(sf::IntRect(
			{ static_cast<int>(std::floor(static_cast<float>(1) * m_Size.x)), 0 },
			{ static_cast<int>(m_Size.x), static_cast<int>(m_Size.y) }
		));
	}

	sf::Sprite Animation::MakeSpriteFromHandle(const ResourceHandle& handle)
	{
		auto tex = std::static_pointer_cast<Luden::Texture>(
			Project::GetResourceManager()->GetResource(handle)
		);
		return sf::Sprite(tex->GetTexture());
	}

}
