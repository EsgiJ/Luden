#include "Scene/Prefab.h"

#include "Scene/Scene.h"
#include "Resource/ResourceImporter.h"
#include "Resource/ResourceManager.h"
/*
namespace Luden {

	Entity Prefab::CreatePrefabFromEntity(Entity entity)
	{
		Entity newEntity = m_Scene->CreateEntity();

		// Add PrefabComponent
		newEntity.AddComponent<PrefabComponent>(Handle, newEntity.UUID());

		entity.m_Scene->CopyComponentIfExists<TransformComponent>(newEntity, m_Scene->m_Registry, entity);
		entity.m_Scene->CopyComponentIfExists<SpriteRendererComponent>(newEntity, m_Scene->m_Registry, entity);
		entity.m_Scene->CopyComponentIfExists<TextComponent>(newEntity, m_Scene->m_Registry, entity);
		entity.m_Scene->CopyComponentIfExists<RigidBody2DComponent>(newEntity, m_Scene->m_Registry, entity);
		entity.m_Scene->CopyComponentIfExists<BoxCollider2DComponent>(newEntity, m_Scene->m_Registry, entity);
		entity.m_Scene->CopyComponentIfExists<CircleCollider2DComponent>(newEntity, m_Scene->m_Registry, entity);

		for (auto childId : entity.Children())
		{
			Entity childDuplicate = CreatePrefabFromEntity(entity.m_Scene->GetEntityWithUUID(childId));

			childDuplicate.SetParentUUID(newEntity.GetUUID());
			newEntity.Children().push_back(childDuplicate.GetUUID());
		}

		if (newEntity.HasComponent<ScriptComponent>())
		{
			const auto& scriptComponent = newEntity.GetComponent<ScriptComponent>();
			newEntity.m_Scene->m_ScriptStorage.InitializeEntityStorage(scriptComponent.ScriptID, newEntity.GetUUID());
			entity.m_Scene->m_ScriptStorage.CopyEntityStorage(entity.GetUUID(), newEntity.GetUUID(), newEntity.m_Scene->m_ScriptStorage);
		}

		return newEntity;
	}

	Prefab::Prefab()
	{
		m_Scene = Scene::CreateEmpty();
	}

	Prefab::~Prefab()
	{
	}

	void Prefab::Create(Entity entity, bool serialize)
	{
		m_Scene = Scene::CreateEmpty();
		m_Entity = CreatePrefabFromEntity(entity);

		if (serialize)
			ResourceImporter::Serialize(this);
	}

	std::unordered_set<ResourceHandle> Prefab::GetResourceList(bool recursive)
	{
		std::unordered_set<ResourceHandle> prefabResourceList = m_Scene->GetResourceList();
		if (!recursive)
			return prefabResourceList;

		for (ResourceHandle handle : prefabResourceList)
		{
			if (!ResourceManager::IsResourceHandleValid(handle))
				continue;

			const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
			if (metadata.Type == ResourceType::Prefab)
			{
				std::shared_ptr<Prefab> prefab = ResourceManager::GetResource<Prefab>(handle);
				std::unordered_set<ResourceHandle> childPrefabResourceList = prefab->GetResourceList(true);
				prefabResourceList.insert(childPrefabResourceList.begin(), childPrefabResourceList.end());
			}
		}

		return prefabResourceList;
	}

}
*/