#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"

#include <functional>

namespace Luden
{
	class ScriptableEntity;

	using ScriptInstantiateFunc = ScriptableEntity* (*)();
	using ScriptDestroyFunc = void(*)(ScriptableEntity*);

	class ENGINE_API NativeScript : public Resource
	{
	public:
		NativeScript(const std::string& name, ScriptInstantiateFunc instantiate, ScriptDestroyFunc destroy)
			: m_Name(name)
			, m_InstantiateFunc(instantiate)
			, m_DestroyFunc(destroy)
		{}

		~NativeScript() override = default;

		static ResourceType GetStaticType() { return ResourceType::NativeScript; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

		const std::string& GetName() { return m_Name; }

		ScriptInstantiateFunc GetInstantiateFunc() const { return m_InstantiateFunc; }
		ScriptDestroyFunc GetDestroyFunc() const { return m_DestroyFunc; }
	private:
		std::string m_Name;
		ScriptInstantiateFunc m_InstantiateFunc;
		ScriptDestroyFunc m_DestroyFunc;
	};
}