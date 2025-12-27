#pragma once

#include "Core/TimeStep.h"
#include "ECS/Entity.h"
#include "EngineAPI.h"
#include "Physics2D/CollisionContact.h"

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