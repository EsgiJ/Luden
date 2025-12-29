#pragma once

#include "Resource/Resource.h"
#include "Resource/ResourceTypes.h"
#include "Scene/Scene.h"

namespace Luden {

	class Prefab : public Resource
	{
	public:
		// Create prefab with empty entity
		Prefab();
		~Prefab();

		// Replaces existing entity if present
		void Create(Entity entity, bool serialize = true);

		static ResourceType GetStaticType() { return ResourceType::Prefab; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

		std::unordered_set<ResourceHandle> GetResourceList(bool recursive = true);
	private:
		Entity CreatePrefabFromEntity(Entity entity);

	private:
		std::shared_ptr<Scene> m_Scene;
		Entity m_Entity;

		friend class Scene;
	};

}
