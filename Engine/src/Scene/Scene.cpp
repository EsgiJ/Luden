#include "Scene/Scene.h"
#include "Core/GameEngine.h"
#include <SFML/Graphics.hpp>

namespace Luden
{
	void Scene::SetPaused(bool paused)
	{
		m_Paused = paused;
	}

	void Scene::SetViewportSize(const sf::Vector2u& size)
	{
		m_ViewportSize = size;
		m_View.setSize({ static_cast<float>(size.x), static_cast<float>(size.y) });
		m_View.setCenter({ static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f });
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
		if (GameEngine::Get().GetWindow().isOpen())
			return static_cast<float>(GameEngine::Get().GetWindow().getSize().x);
		return 0.0f;
    }

    float Scene::Height() const
    {
            if (m_ViewportSize.y != 0)
                    return static_cast<float>(m_ViewportSize.y);
            if (GameEngine::Get().GetWindow().isOpen())
                    return static_cast<float>(GameEngine::Get().GetWindow().getSize().y);
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

		GameEngine::Get().GetWindow().draw(line, 2, sf::PrimitiveType::Lines);
	}

	std::unordered_set<ResourceHandle> Scene::GetResourceList()
	{
		std::unordered_set<ResourceHandle> resourceList;

		for (auto& entity: m_EntityManager.GetEntities())
		{
			//Animation
			for (auto animation: Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Animation))
			{
				resourceList.insert(animation);
			}

			//Font 
			for (auto font : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Font))
			{
				resourceList.insert(font);
			}

			//CDamage Component
			for (auto audio : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Audio))
			{
				resourceList.insert(audio);
			}

			//CDamage Component
			for (auto scene : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Scene))
			{
				resourceList.insert(scene);
			}

			//CDamage Component
			for (auto texture : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Texture))
			{
				resourceList.insert(texture);
			}
		}

		return resourceList;
	}

	Entity Scene::CreateEntity(const std::string& tag)
	{
		return m_EntityManager.AddEntity(tag);
	}

	void Scene::DestroyEntity(const Entity& entity)
	{
		DestroyEntity(entity.UUID());
	}

	void Scene::DestroyEntity(const UUID& entityID)
	{
		m_EntityManager.DestroyEntity(entityID);
	}
	//TODO:
	/*
	Entity Scene::DuplicateEntity(const Entity& entity)
	{
		return Entity();
	}

	Entity Scene::GetEntityWithUUID(const UUID& uuid) const
	{
		return Entity();
	}

	Entity Scene::TryGetEntityWithUUID(const UUID& uuid)
	{
		return Entity();
	}

	Entity Scene::TryGetEntityWithTag(const std::string& tag)
	{
		return Entity();
	}
	*/
}
