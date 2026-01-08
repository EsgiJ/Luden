#include "Scene/Prefab.h"

#include "Scene/Scene.h"
#include "Resource/ResourceImporter.h"
#include "Resource/ResourceManager.h"
#include <iostream>

namespace Luden {

    Entity Prefab::CreatePrefabFromEntity(Entity entity)
    {
		Entity newEntity = m_Scene->CreateEntityImmediate(entity.Tag());

        auto sourceScene = entity.GetScene();

        sourceScene->CopyComponentIfExists<RelationshipComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<DamageComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<DraggableComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<FollowPLayerComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<GravityComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<HealthComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<InputComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<Camera2DComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<RigidBody2DComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<SpriteAnimatorComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<TextComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<InvincibilityComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<LifespanComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<PatrolComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<StateComponent>(newEntity, entity);
        sourceScene->CopyComponentIfExists<TransformComponent>(newEntity, entity);

		if (newEntity.Has<RelationshipComponent>())
		{
			auto& relComp = newEntity.Get<RelationshipComponent>();
			relComp.Children.clear();  
			relComp.ParentHandle = 0;  
		}

        for (auto childId : entity.Children())
        {
            Entity childEntity = sourceScene->GetEntityWithUUID(childId);
            Entity childDuplicate = CreatePrefabFromEntity(childEntity);

            if (childDuplicate.IsValid())
            {
                childDuplicate.SetParentUUID(newEntity.UUID());
                newEntity.Children().push_back(childDuplicate.UUID());
            }
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
			ResourceImporter::Serialize(shared_from_this());
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
