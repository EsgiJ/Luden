#pragma once

#include "EngineAPI.h"
#include "Resource/ResourceManagerBase.h"

namespace Luden
{
	class IScriptModule
	{
	public:
		IScriptModule() = default;
		virtual ~IScriptModule() = default;

		virtual void OnLoad() = 0;

		virtual void OnUnload() = 0;

		virtual void RegisterScripts(ResourceManagerBase* resourceManager) = 0;

		virtual uint32_t GetVersion() const = 0;
	};

#ifdef GAME_MODULE_EXPORTS
	#define GAME_MODULE_API __declspec(dllexport)
#else
	#define  GAME_MODULE_API __declspec(dllimport)
#endif

	extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule();
	extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module);
}