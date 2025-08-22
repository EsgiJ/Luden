#pragma once

#include "EngineAPI.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"
#include "Resource/Resource.h"
#include "Graphics/Texture.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>

namespace Luden::Graphics
{
	class ENGINE_API Animation : public Resource
	{
	public:
		Animation(const std::string& name, const ResourceHandle& textureHandle);
		Animation(const std::string& name, const ResourceHandle& textureHandle, size_t frameCount, size_t speed);

		void Update();
		bool HasEnded() const;

		const sf::Sprite& GetSprite() const { return m_Sprite; }
		sf::Sprite& GetSprite() { return m_Sprite; }
		void SetSprite(const sf::Sprite& sprite) { m_Sprite = sprite; }

		const size_t GetFrameCount() const { return m_FrameCount; }
		void SetFrameCount(size_t frameCount) { m_FrameCount = frameCount; }

		const size_t GetSpeed() const { return m_Speed; }
		void SetSpeed(size_t speed) { m_Speed = speed; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const Math::Vec2& GetSize() const { return m_Size; }
		void SetSize(const Math::Vec2& size) { m_Size = size; }

		const ResourceHandle& GetTextureHandle() const { return m_TextureHandle; }
		ResourceHandle GetTextureHandle() { return m_TextureHandle; }
		void SetTextureHandle(const ResourceHandle& textureHandle) { m_TextureHandle = textureHandle; }

		static sf::Sprite MakeSpriteFromHandle(const ResourceHandle& handle);
	private:
		sf::Sprite m_Sprite;
		size_t m_FrameCount = 1;
		size_t m_CurrentFrame = 0;
		size_t m_Speed = 0;
		Math::Vec2 m_Size = { 1, 1 };
		std::string m_Name = "None";
		ResourceHandle m_TextureHandle;
	};
}
