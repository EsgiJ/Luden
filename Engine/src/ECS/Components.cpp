#include "ECS/Components/Components.h"
#include "NativeScript/ScriptableEntity.h"
#include "NativeScript/NativeScript.h"
#include "Project/Project.h"

namespace Luden
{
	template<typename T>
	void NativeScriptComponent::Bind()
	{
		InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript = [](ScriptableEntity* se) { delete se; se = nullptr; };
	}

	void NativeScriptComponent::BindFromHandle(ResourceHandle handle)
	{
		if (!Project::GetResourceManager()->IsResourceHandleValid(handle))
			return;

		auto resource = Project::GetResourceManager()->GetResource(handle);
		auto script = std::static_pointer_cast<NativeScript>(resource);

		if (!script)
			return;

		ScriptHandle = handle;
		InstantiateScript = script->GetInstantiateFunc();
		DestroyScript = script->GetDestroyFunc();
	}

	void NativeScriptComponent::CreateInstance(Entity entity)
	{
		if (Instance || ScriptHandle == 0)
			return;

		auto resource = Project::GetResourceManager()->GetResource(ScriptHandle);
		auto script = std::static_pointer_cast<NativeScript>(resource);
		if (!script)
			return;

		Instance = script->GetInstantiateFunc()();
		if (Instance)
		{
			Instance->m_Entity = entity;
			Instance->OnCreate();
		}
	}

	void NativeScriptComponent::DestroyInstance()
	{
		if (!Instance || ScriptHandle == 0)
			return;

		auto resource = Project::GetResourceManager()->GetResource(ScriptHandle);
		auto script = std::static_pointer_cast<NativeScript>(resource);
		if (!script)
			return;

		Instance->OnDestroy();
		script->GetDestroyFunc()(Instance);
	}
}