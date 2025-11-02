#pragma once

#include "NativeScript/NativeScript.h"
#include "EngineAPI.h"

namespace Luden
{
	class ENGINE_API NativeScriptRegistry
	{
	public:
		template<typename T>
		void RegisterScript(const std::string& name, ScriptInstantiateFunc instantiate, ScriptDestroyFunc destroy)
		{
		}
	private:

	};
}