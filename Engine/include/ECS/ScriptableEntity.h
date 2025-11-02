#pragma once

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
	
		protected:
			virtual void OnCreate() {}
			virtual void OnUpdate(TimeStep ts) {}
			virtual void OnDestroy() {}

		private:
			Entity m_Entity;
			friend class Scene;
		};
	}