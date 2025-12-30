#pragma once

#include "Resource/Resource.h"
#include "Resource/ResourceTypes.h"
#include "Scene/Scene.h"

namespace Luden {

	class ENGINE_API Prefab : public Resource, public std::enable_shared_from_this<Prefab>
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

		Entity GetRootEntity() const { return m_Entity; }
		void SetRootEntity(Entity entity) { m_Entity = entity; }

		std::shared_ptr<Scene> GetScene() const { return m_Scene; }
		void SetScene(std::shared_ptr<Scene> scene) { m_Scene = scene; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
	private:
		Entity CreatePrefabFromEntity(Entity entity);

	private:
		std::shared_ptr<Scene> m_Scene;
		Entity m_Entity;
		std::string m_Name;
		friend class Scene;
	};

}
