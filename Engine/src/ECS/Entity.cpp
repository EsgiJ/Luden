#include "ECS/Entity.h"

#include <vector>

#include "ECS/EntityManager.h"

namespace Luden
{
	Entity::Entity(EntityID id) 
		: m_ID(id) {}

	bool Entity::IsActive() const 
	{
		return EntityManager::Instance().IsActive(m_ID);
	}

	const std::string& Entity::Tag() const 
	{
		return EntityManager::Instance().GetTag(m_ID);
	}

	size_t Entity::Id() const 
	{
		return m_ID;
	}

	void Entity::Destroy() 
	{
		EntityManager::Instance().DestroyEntity(m_ID);
	}
}
