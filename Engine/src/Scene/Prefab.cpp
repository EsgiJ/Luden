#include "Scene/Prefab.h"

#include "Scene/Scene.h"
#include "Resource/ResourceImporter.h"
#include "Resource/ResourceManager.h"

namespace Luden {

	Entity Prefab::CreatePrefabFromEntity(Entity entity)
	{
		Entity newEntity = m_Scene->CreateEntity();

		// Add PrefabComponent
		//newEntity.AddComponent<PrefabComponent>(Handle, newEntity.UUID());

		entity.GetScene()->CopyComponentIfExists<RelationshipComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<DamageComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<DraggableComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<FollowPLayerComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<GravityComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<HealthComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<InputComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<Camera2DComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<RigidBody2DComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<SpriteAnimatorComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<TextComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<InvincibilityComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<LifespanComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<PatrolComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<StateComponent>(newEntity, entity);
		entity.GetScene()->CopyComponentIfExists<TransformComponent>(newEntity, entity);

		for (auto childId : entity.Children())
		{
			Entity childDuplicate = CreatePrefabFromEntity(entity.GetScene()->GetEntityWithUUID(childId));

			childDuplicate.SetParentUUID(newEntity.UUID());
			newEntity.Children().push_back(childDuplicate.UUID());
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
		{
			//ResourceImporter::Serialize(this);

		}
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
