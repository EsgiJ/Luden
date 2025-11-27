#pragma once

#include "EngineAPI.h"

#include <glm/vec3.hpp>
#include <SFML/Graphics/Color.hpp>

namespace Luden
{
	namespace DebugAPI
	{
		void ENGINE_API DrawDebugLine(const glm::vec3& start, const glm::vec3& end, const sf::Color& color, float duration);

		void ENGINE_API DrawDebugCircle(const glm::vec3& center, float radius, const sf::Color& color, float duration);

		void ENGINE_API DrawDebugBox(const glm::vec3& center, const glm::vec3& extent, const sf::Color& color, float duration);
	}
}