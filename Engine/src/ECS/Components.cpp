#include "ECS/Components/Components.h"
#include "NativeScript/ScriptableEntity.h"
#include "NativeScript/NativeScript.h"
#include "Project/Project.h"
#include "Resource/ResourceManager.h"

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

		auto script = ResourceManager::GetResource<NativeScript>(handle);

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

		auto script = ResourceManager::GetResource<NativeScript>(ScriptHandle); 
		if (!script)
			return;

		if (!(script->GetInstantiateFunc()))
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
		if (!Instance)
			return;

		if (ScriptHandle == 0)
		{
			delete Instance;
			Instance = nullptr;
			return;
		}

		auto script = ResourceManager::GetResource<NativeScript>(ScriptHandle);

		if (!script)
		{
			delete Instance;
			Instance = nullptr;
			return;
		}

		Instance->OnDestroy();

		if (!(script->GetDestroyFunc()))
			return;

		script->GetDestroyFunc()(Instance);
		Instance = nullptr; 
	}
}