#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "EngineAPI.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden::Graphics
{
	class ENGINE_API Animation
	{
		RTTR_REGISTRATION_FRIEND

		sf::Sprite m_Sprite;
		size_t m_FrameCount = 1;
		size_t m_CurrentFrame = 0;
		size_t m_Speed = 0;
		Math::Vec2 m_Size = { 1, 1 };
		std::string m_Name = "none";
		sf::Texture m_Texture;
	public:
		Animation();
		Animation(const std::string& name, const sf::Texture& texture);
		Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed);

		void Update();
		bool HasEnded() const;

		const std::string& GetName() const;
		const Math::Vec2& GetSize() const;

		const sf::Sprite& GetSprite() const;
		sf::Sprite& GetSprite();
	};
}
