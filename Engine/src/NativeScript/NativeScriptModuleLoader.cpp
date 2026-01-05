#include "NativeScript/NativeScriptModuleLoader.h"
#include "IO/FileSystem.h"
#include <iostream>
#include "Project/Project.h"
#include <thread>

namespace Luden
{
	NativeScriptModuleLoader::~NativeScriptModuleLoader()
	{
		UnloadModule();
	}

	bool NativeScriptModuleLoader::LoadModule(const std::filesystem::path& dllPath)
	{
		if (IsLoaded())
		{
			std::cerr << "Module already loaded!" << std::endl;
			return false;
		}

		m_ModulePath = dllPath;
		return LoadModuleInternal(dllPath);
	}

	bool NativeScriptModuleLoader::LoadModuleInternal(const std::filesystem::path& path)
	{
		auto now = std::chrono::system_clock::now();
		auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		std::string moduleName = path.stem().string();

		m_TempDllPath = std::filesystem::temp_directory_path() / (moduleName + "_" + std::to_string(timestamp) + ".dll");

		m_TempPdbPath = m_TempDllPath;
		m_TempPdbPath.replace_extension(".pdb");

		std::filesystem::path originalPdbPath = path;
		originalPdbPath.replace_extension(".pdb");

		try
		{
			std::filesystem::copy_file(path, m_TempDllPath,
				std::filesystem::copy_options::overwrite_existing);

			if (std::filesystem::exists(originalPdbPath))
			{
				std::filesystem::copy_file(
					originalPdbPath,
					m_TempPdbPath,
					std::filesystem::copy_options::overwrite_existing
				);
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Failed to copy DLL: " << e.what() << std::endl;
			return false;
		}

		m_ModuleHandle = LoadLibraryW(m_TempDllPath.wstring().c_str());
		if (!m_ModuleHandle)
		{
			std::cerr << "Failed to load DLL: " << GetLastError() << std::endl;
			return false;
		}

		auto createFunc = (CreateModuleFunc)GetProcAddress(m_ModuleHandle, "CreateScriptModule");
		if (!createFunc)
		{
			std::cerr << "Failed to find CreateScriptModule function!" << std::endl;
			FreeLibrary(m_ModuleHandle);
			m_ModuleHandle = nullptr;
			return false;
		}

		m_Module = createFunc();
		if (!m_Module)
		{
			std::cerr << "Failed to create module instance!" << std::endl;
			FreeLibrary(m_ModuleHandle);
			m_ModuleHandle = nullptr;
			return false;
		}

		m_Module->OnLoad();
		return true;
	}

	void NativeScriptModuleLoader::UnloadModule()
	{
		if (!IsLoaded())
			return;

		m_Module->OnUnload();

		auto destroyFunc = (DestroyModuleFunc)GetProcAddress(m_ModuleHandle, "DestroyScriptModule");
		if (destroyFunc)
		{
			destroyFunc(m_Module);
		}

		m_Module = nullptr;

		if (m_ModuleHandle)
		{
			FreeLibrary(m_ModuleHandle);
			m_ModuleHandle = nullptr;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		if (std::filesystem::exists(m_TempDllPath))
			std::filesystem::remove(m_TempDllPath);

		if (std::filesystem::exists(m_TempPdbPath))
			std::filesystem::remove(m_TempPdbPath);
	}

	bool NativeScriptModuleLoader::ReloadModule()
	{
		if (!IsLoaded())
			return false;

		std::cout << "Hot reloading module..." << std::endl;

		UnloadModule();

		return LoadModuleInternal(m_ModulePath);
	}
}