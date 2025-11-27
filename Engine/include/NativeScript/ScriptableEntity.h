#pragma once

#include "Core/TimeStep.h"
#include "ECS/Entity.h"
#include "EngineAPI.h"

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
	
		protected:
			virtual void OnCreate() {}
			virtual void OnUpdate(TimeStep ts) {}
			virtual void OnDestroy() {}

		private:
			Entity m_Entity;
			friend struct NativeScriptComponent;
			friend class Scene;
		};
	}