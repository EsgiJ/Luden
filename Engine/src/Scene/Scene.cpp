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
                if (m_ViewportSize.x != 0)
                        return static_cast<float>(m_ViewportSize.x);
                if (m_Game && m_Game->GetWindow().isOpen())
                        return static_cast<float>(m_Game->GetWindow().getSize().x);
                return 0.0f;
        }

        float Scene::Height() const
        {
                if (m_ViewportSize.y != 0)
                        return static_cast<float>(m_ViewportSize.y);
                if (m_Game && m_Game->GetWindow().isOpen())
                        return static_cast<float>(m_Game->GetWindow().getSize().y);
                return 0.0f;
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

	EntityManager& Scene::GetEntityManager()
	{
		return m_EntityManager;
	}

	const EntityManager& Scene::GetEntityManager() const
	{
		return m_EntityManager;
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
