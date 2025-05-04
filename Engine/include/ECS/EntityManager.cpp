#include "ECS/EntityManager.h"

namespace Luden
{
	Entity EntityManager::AddEntity(const std::string& tag) {
		Entity e = EntityMemoryPool::Instance().AddEntity(tag);
		m_entitiesToAdd.push_back(e);
		return e;
	}
}

