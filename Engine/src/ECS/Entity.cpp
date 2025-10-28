#include "ECS/Entity.h"
#include "ECS/EntityMemoryPool.h"
#include "Scene/Scene.h"

#include <vector>
namespace Luden
{
	Entity::Entity(EntityID uuid, Scene* scene)
		: m_UUID(uuid), m_Scene(scene) {
	}

	bool Entity::IsActive() const 
	{
		if (!EntityMemoryPool::Instance().Exists(m_UUID))
		{
			return false;
		}

		return EntityMemoryPool::Instance().IsActive(m_UUID);
	}

	void Entity::SetTag(const std::string& tag)
	{
		return EntityMemoryPool::Instance().SetTag(m_UUID, tag);
	}

	const std::string& Entity::Tag() const
	{
		if (!EntityMemoryPool::Instance().Exists(m_UUID))
		{
			static const std::string invalidTag = "Invalid Entity";
			return invalidTag;
		}

		return EntityMemoryPool::Instance().GetTag(m_UUID);
	}


	void Entity::SetParent(Entity parent)
	{
		Entity currentParent = GetParent();

		if (parent == currentParent)
			return;

		if (currentParent)
			currentParent.RemoveChild(*this);

		SetParentUUID(parent.UUID());
	}

	Entity Entity::GetParent()
	{
		return m_Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	void Entity::SetParentUUID(EntityID parent)
	{
		Get<RelationshipComponent>().ParentHandle = parent;
	}

	EntityID Entity::GetParentUUID() const
	{
		return Get<RelationshipComponent>().ParentHandle;
	}

	std::vector<EntityID>& Entity::Children()
	{
		return Get<RelationshipComponent>().Children;
	}

	const std::vector<EntityID>& Entity::Children() const
	{
		return Get<RelationshipComponent>().Children;
	}

	bool Entity::RemoveChild(Entity child)
	{
		EntityID childID = child.UUID();
		std::vector<EntityID> children = Children();
		
		auto it = std::find(children.begin(), children.end(), childID);

		if (it != children.end())
		{
			children.erase(it);
			return true;
		}

		return false;
	}

	bool Entity::IsAncestorOf(Entity& entity) const
	{
		const auto& children = Children();

		if (children.empty())
			return false;

		for (const EntityID child : children)
		{
			if (child == entity.UUID())
				return true;
		}

		for (const EntityID child : children)
		{
			if (m_Scene->GetEntityWithUUID(child).IsAncestorOf(entity))
				return true;
		}

		return false;
	}

	void Entity::Destroy() 
	{
		EntityMemoryPool::Instance().SetActive(m_UUID, false);
	}
}
