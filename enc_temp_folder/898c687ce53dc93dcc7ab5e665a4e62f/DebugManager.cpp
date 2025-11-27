#include "Debug/DebugManager.h"

#include <SFML/Graphics.hpp>
#include <iostream>

namespace Luden
{
	void DebugManager::DrawLine(const glm::vec3& start, const glm::vec3& end, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;

		cmd.type = DebugDrawCommand::Type::Line;
		cmd.p1 = start;
		cmd.p2 = end;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}

	void DebugManager::DrawCircle(const glm::vec3& center, float radius, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;
		cmd.type = DebugDrawCommand::Type::Circle;
		cmd.p1 = center;
		cmd.radius = radius;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}

	void DebugManager::DrawBox(const glm::vec3& center, const glm::vec3& extent, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;
		cmd.type = DebugDrawCommand::Type::Box;
		cmd.p1 = center;
		cmd.p2 = extent;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}
	void DebugManager::Update(float deltaTime)
	{
		for (auto it = m_Commands.begin(); it != m_Commands.end();)
		{
			if (it->duration > 0.0f)
			{
				it->timeRemaining -= deltaTime;
				if (it->timeRemaining <= 0.0f)
				{
					it = m_Commands.erase(it);
					continue;
				}
			}
			it++;
		}
	}

	void DebugManager::Render(std::shared_ptr<sf::RenderTexture> target)
	{
		for (const auto& cmd : m_Commands)
		{
			sf::Color sfColor(
				(cmd.color.r),
				(cmd.color.g),
				(cmd.color.b),
				(cmd.color.a)
			);

			std::cout << "Alpha: " << (int)cmd.color.a << std::endl;


			switch (cmd.type)
			{
			case DebugDrawCommand::Type::Line:
			{
				sf::Vertex line[2] = {
					sf::Vertex(sf::Vector2f(cmd.p1.x, cmd.p1.y), sfColor),
					sf::Vertex(sf::Vector2f(cmd.p2.x, cmd.p2.y), sfColor)
				};

				target->draw(line, 2, sf::PrimitiveType::Lines);
				break;
			}
			case DebugDrawCommand::Type::Circle:
			{
				sf::CircleShape circle(cmd.radius);
				circle.setPosition(sf::Vector2f(cmd.p1.x - cmd.radius, cmd.p1.y - cmd.radius));
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineColor(sfColor);
				circle.setOutlineThickness(8.0f);
				target->draw(circle);
				break;
			}

			case DebugDrawCommand::Type::Box:
			{
				sf::RectangleShape box(sf::Vector2f(cmd.p2.x * 2, cmd.p2.y * 2));
				box.setPosition(sf::Vector2f(cmd.p1.x - cmd.p2.x, cmd.p1.y - cmd.p2.y));
				box.setFillColor(sf::Color::Transparent);
				box.setOutlineColor(sfColor);
				box.setOutlineThickness(8.0f);
				target->draw(box);
				break;
			}
			default:
				break;
			}
		}
	}
	void DebugManager::Clear()
	{
		m_Commands.clear();
	}
}