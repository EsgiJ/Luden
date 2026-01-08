#pragma once

#include "Core/TimeStep.h"
#include "ECS/Entity.h"
#include "EngineAPI.h"
#include "Luden.h"
#include "Resource/ResourceManager.h"  
#include "Project/Project.h"  
#include "Physics2D/CollisionContact.h"
#include "Utilities/StringUtils.h"

	namespace Luden
	{
		class Entity;
		class TimeStep;

		class ENGINE_API ScriptableEntity
		{
		public:
			virtual ~ScriptableEntity() {}

			template<typename T>
			T& GetComponent()
			{
				return m_Entity.Get<T>();
			}

			template<typename T>
			bool HasComponent()
			{
				return m_Entity.Has<T>();
			}

			template<typename T>
			T& AddComponent()
			{
				return m_Entity.Add<T>();
			}
			
			template<typename T>
			std::shared_ptr<T> GetResource(const std::string& name)
			{
				auto handles = ResourceManager::template GetAllResourcesWithType<T>();

				for (auto handle : handles)
				{
					const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
					std::string filename = RemoveExtension(metadata.FilePath.filename().string());

					if (EqualsIgnoreCase(filename, name))
					{
						return ResourceManager::template GetResource<T>(handle);
					}
				}
				return nullptr;
			}

			Entity GetEntity() { return m_Entity; }
		protected:
			//Life cycle
			virtual void OnCreate() {}
			virtual void OnUpdate(TimeStep ts) {}
			virtual void OnDestroy() {}

			//Physics2D
			virtual void OnCollisionBegin(const CollisionContact& contact) {}
			virtual void OnCollisionEnd(const CollisionContact& contact) {}
			virtual void OnCollisionHit(const CollisionContact& contact) {}
		private:
			Entity m_Entity;
			friend struct NativeScriptComponent;
			friend class Scene;
			friend class Physics2DManager;
		};
	}