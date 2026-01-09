#pragma once
#include "Resource/Resource.h"

#include <SFML/Graphics/Rect.hpp>
#include <glm/vec2.hpp>

namespace Luden
{
	class Texture;

	class ENGINE_API Sprite : public Resource
	{
	public:
		Sprite() = default;
		~Sprite() = default;

		ResourceHandle GetTextureHandle() const { return m_TextureHandle; }
		void SetTextureHandle(ResourceHandle handle) { m_TextureHandle = handle; }

		const sf::IntRect& GetTextureRect() const { return m_TextureRect; }
		void SetTextureRect(const sf::IntRect& rect) { m_TextureRect = rect; }
		sf::Vector2u GetSize();


		std::shared_ptr<Texture> GetTexture();

		bool UsesFullTexture() const {
			return m_TextureRect.size.x == 0 && m_TextureRect.size.y == 0;
		}

		const glm::vec2& GetPivot() const { return m_Pivot; }
		void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }

		static ResourceType GetStaticType() { return ResourceType::Sprite; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

	private:
		ResourceHandle m_TextureHandle = 0;
		sf::IntRect m_TextureRect = { {0, 0}, {0, 0} };  
		glm::vec2 m_Pivot = { 0.5f, 0.5f };              

		friend class SpriteSerializer;
	};
}