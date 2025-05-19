#include "Graphics/Animation.h"

#include <cmath>
#include <utility>


namespace Luden::Graphics
{
	Animation::Animation()
		: m_Sprite(m_Texture)
	{
	}
	Animation::Animation(const std::string& name, const sf::Texture& texture)
		: m_Name(name)
		, m_Sprite(texture)
		, m_FrameCount(1)
		, m_CurrentFrame(0)
		, m_Speed(0) { }

	Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed)
		: m_Name(name)
		, m_Sprite(texture)
		, m_FrameCount(frameCount)
		, m_CurrentFrame(0)
		, m_Speed(speed)
	{
		m_Size = Math::Vec2(
			static_cast<float>(texture.getSize().x) / static_cast<float>(frameCount),
			static_cast<float>(texture.getSize().y)
		);

		m_Sprite.setTexture(texture);
		m_Sprite.setOrigin(sf::Vector2f(m_Size.x / 2.0f, m_Size.y / 2.0f));

		m_Sprite.setTextureRect(sf::IntRect(
			{ static_cast<int>(std::floor(static_cast<float>(m_CurrentFrame) * m_Size.x)), 0 },
			{ static_cast<int>(m_Size.x), static_cast<int>(m_Size.y) }
		));
	}

	void Animation::Update()
	{
		m_CurrentFrame++;
		if (m_Speed == 0 || m_FrameCount == 0)
			return;
		size_t frame = (m_CurrentFrame / m_Speed) % m_FrameCount;
		m_Sprite.setTextureRect(sf::IntRect(
			{ static_cast<int>(frame * m_Size.x), 0 },
			{ static_cast<int>(m_Size.x), static_cast<int>(m_Size.y) }
		));
	}

	bool Animation::HasEnded() const
	{
		if (m_Speed == 0) return true;

		return (m_CurrentFrame / m_Speed) % m_FrameCount == m_FrameCount - 1;
	}

	const std::string& Animation::GetName() const
	{
		return m_Name;
	}

	const Math::Vec2& Animation::GetSize() const
	{
		return m_Size;
	}

	const sf::Sprite& Animation::GetSprite() const
	{
		return m_Sprite;
	}

	sf::Sprite& Animation::GetSprite()
	{
		return m_Sprite;
	}
}
