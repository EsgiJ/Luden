#include "Scene/Scene.h"
#include "Core/GameEngine.h"
#include <SFML/Graphics.hpp>

namespace Luden
{
	Scene::Scene(GameEngine* gameEngine)
		: m_Game(gameEngine) {
	}

	void Scene::SetPaused(bool paused)
	{
		m_Paused = paused;
	}

	void Scene::DoAction(const Action& action)
	{
		sDoAction(action);
	}

	void Scene::Simulate(size_t frames)
	{
		for (size_t i = 0; i < frames; i++)
		{
			Update();
			m_CurrentFrame++;
		}
	}

	void Scene::RegisterAction(sf::Keyboard::Key inputKey, const std::string& actionName)
	{
		m_ActionMap[static_cast<int>(inputKey)] = actionName;
	}

	float Scene::Width() const
	{
		return static_cast<float>(m_Game->GetWindow().getSize().x);
	}

	float Scene::Height() const
	{
		return static_cast<float>(m_Game->GetWindow().getSize().y);
	}

	size_t Scene::CurrentFrame() const
	{
		return m_CurrentFrame;
	}

	bool Scene::HasEnded() const
	{
		return m_HasEnded;
	}

	const ActionMap& Scene::GetActionMap() const
	{
		return m_ActionMap;
	}
	
	void Scene::DrawLine(const Math::Vec2& p1, const Math::Vec2& p2)
	{
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(p1.x, p1.y)),
			sf::Vertex(sf::Vector2f(p2.x, p2.y))
		};

		m_Game->GetWindow().draw(line, 2, sf::PrimitiveType::Lines);
	}
}
