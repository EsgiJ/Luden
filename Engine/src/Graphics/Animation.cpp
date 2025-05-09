#include <cmath>
#include <utility>
#include "Graphics/Animation.h"

namespace Luden::Graphics
{
	Animation::Animation() = default;

	Animation::Animation(const std::string& name, const sf::Texture& texture)
		: Animation(name, texture, 1, 0) {
	}

	Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed)
		: m_Name(name), m_Sprite(texture), m_FrameCount(frameCount), m_CurrentFrame(0), m_Speed(speed)
	{
		m_Size = Math::Vec2(
			static_cast<float>(texture.getSize().x) / static_cast<float>(frameCount),
			static_cast<float>(texture.getSize().y)
		);
		m_Sprite.setOrigin(sf::Vector2f(m_Size.x / 2.0f, m_Size.y / 2.0f));
		m_Sprite.setTextureRect(sf::IntRect(
			sf::Vector2i(static_cast<int>(m_CurrentFrame * m_Size.x), 0),			// Origin
			sf::Vector2i(static_cast<int>(m_Size.x), static_cast<int>(m_Size.y)))	// Width, Height
		);
	}

	void Animation::Update()
	{
		m_CurrentFrame++;
		size_t frame = (m_CurrentFrame / m_Speed) % m_FrameCount;
		m_Sprite.setTextureRect(sf::IntRect(
			sf::Vector2i(static_cast<int>(frame * m_Size.x), 0),					// Origin
			sf::Vector2i(static_cast<int>(m_Size.x), static_cast<int>(m_Size.y)))	// Width, Height
		);
	}

	bool Animation::HasEnded() const
	{
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
