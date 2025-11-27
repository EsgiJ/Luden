#include "ScriptAPI/DebugAPI.h"
#include "Debug/DebugManager.h"

#include <SFML/Graphics/Color.hpp>

namespace Luden
{
	namespace DebugAPI
	{
		void DrawDebugLine(const glm::vec3& start, const glm::vec3& end, const sf::Color& color, float duration)
		{
			DebugManager::Instance().DrawLine(start, end, color, duration);
		}

		void DrawDebugCircle(const glm::vec3& center, float radius, const sf::Color& color, float duration)
		{
			DebugManager::Instance().DrawCircle(center, radius, color, duration);
		}

		void DrawDebugBox(const glm::vec3& center, const glm::vec3& extent, const sf::Color& color, float duration)
		{
			DebugManager::Instance().DrawBox(center, extent, color, duration);
		}
	}
}