#pragma once
#include "EngineAPI.h"
#include <cstdint>

namespace Luden
{
	class ResourceManagerBase;
	class RuntimeApplication;
	struct ApplicationSpecification;

	class ENGINE_API IScriptModule
	{
	public:
		virtual ~IScriptModule() = default;

		virtual void OnLoad() = 0;
		virtual void OnUnload() = 0;
		virtual void RegisterScripts(ResourceManagerBase* resourceManager) = 0;
		virtual uint32_t GetVersion() const = 0;
		virtual RuntimeApplication* CreateRuntimeApplication(const ApplicationSpecification& spec) { return nullptr; }
	};
}

#ifdef _WIN32
#define GAME_MODULE_API __declspec(dllexport)
#else
#define GAME_MODULE_API __attribute__((visibility("default")))
#endif

extern "C" GAME_MODULE_API Luden::IScriptModule* CreateScriptModule();
extern "C" GAME_MODULE_API void DestroyScriptModule(Luden::IScriptModule* module);