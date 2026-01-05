#pragma once
#include "NativeScript/IScriptModule.h"
#include <Windows.h>
#include <filesystem>

namespace Luden
{
	class ENGINE_API NativeScriptModuleLoader
	{
	public:
		NativeScriptModuleLoader() = default;
		~NativeScriptModuleLoader();

		bool LoadModule(const std::filesystem::path& dllPath);

		void UnloadModule();

		bool ReloadModule();

		IScriptModule* GetModule() const { return m_Module; }

		bool IsLoaded() const { return m_Module != nullptr; }

		const std::filesystem::path& GetModulePath() const { return m_ModulePath; }

	private:
		bool LoadModuleInternal(const std::filesystem::path& path);

		HMODULE m_ModuleHandle = nullptr;
		IScriptModule* m_Module = nullptr;
		std::filesystem::path m_ModulePath;
		std::filesystem::path m_TempDllPath;
		std::filesystem::path m_TempPdbPath;
		uint32_t m_LoadCount = 0;

		typedef IScriptModule* (*CreateModuleFunc)();
		typedef void (*DestroyModuleFunc)(IScriptModule*);
	};
}