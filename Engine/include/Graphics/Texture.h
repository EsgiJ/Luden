#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include <SFML/Graphics/Texture.hpp>

namespace Luden
{
	class ENGINE_API Texture : public Resource
	{
	public:
		Texture() = default;
		~Texture() override = default;

		const sf::Texture& GetTexture() const { return m_Texture; }
		sf::Texture& GetTexture() { return m_Texture; }

		void SetTexture(const sf::Texture& texture) { m_Texture = texture; }

		static ResourceType GetStaticType() { return ResourceType::Texture; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }
	private:
		sf::Texture m_Texture;
	};
}