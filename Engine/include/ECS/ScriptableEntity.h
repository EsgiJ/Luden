#pragma once

#include "ECS/Entity.h"

namespace Luden
{
	class TimeStep;

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			m_Entity.Get<T>();
		}
	
	protected:
		virtual void OnCreate();
		virtual void OnUpdate(TimeStep ts);
		virtual void OnDestroy();

	private:
		Entity m_Entity;
		friend class Scene;
	};
}