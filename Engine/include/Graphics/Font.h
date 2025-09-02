#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include <SFML/Graphics/Font.hpp>

namespace Luden
{
	class ENGINE_API Font : public Resource
	{
	public:
		Font() = default;
		~Font() override = default;

		const sf::Font& GetFont() const { return m_Font; }
		sf::Font& GetFont() { return m_Font; }

		void SetFont(sf::Font& font) { m_Font = font; }

		static ResourceType GetStaticType() { return ResourceType::Font; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }
	private:
		sf::Font m_Font;
	};
}