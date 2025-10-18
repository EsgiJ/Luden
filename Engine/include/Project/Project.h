#pragma once

#include <filesystem>

#include "EngineAPI.h"
#include "Scene/Scene.h"
#include "Resource/EditorResourceManager.h"
#include "Resource/RuntimeResourceManager.h"

namespace Luden
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::string ResourceDirectory = "Resource";
		std::string ResourceRegistryPath = "Resource/ResourceRegistry.lr";

		std::filesystem::path StartScene = "";

		std::string ProjectFileName;
		std::string ProjectDirectory;

		ResourceHandle StartSceneHandle;
	};

	class ENGINE_API Project
	{
	public:

		Project() = default;
		~Project() = default;

		ProjectConfig& GetConfig() { return m_Config; }

		static std::shared_ptr<Project> GetActiveProject() { return s_ActiveProject; }
		static void SetActive(std::shared_ptr<Project> project);
		static void SetActiveRuntime(std::shared_ptr<Project> project, std::shared_ptr<ResourcePack> resourcePack);

		inline static std::shared_ptr<ResourceManagerBase> GetResourceManager() { return s_ResourceManager; }
		inline static std::shared_ptr<EditorResourceManager> GetEditorResourceManager() { return std::dynamic_pointer_cast<EditorResourceManager>(s_ResourceManager); }
		inline static std::shared_ptr<RuntimeResourceManager> GetRuntimeResourceManager() { return std::dynamic_pointer_cast<RuntimeResourceManager>(s_ResourceManager); }

		static const std::filesystem::path GetProjectDirectory()
		{
			assert(s_ActiveProject != nullptr && "Active project is null. Call Project::SetActive() first.");
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory);
		}

		const std::filesystem::path GetResourceDirectory()
		{
			return std::filesystem::path(GetConfig().ProjectDirectory) / std::filesystem::path((GetConfig().ResourceDirectory));
		}

		static std::filesystem::path GetActiveResourceDirectory()
		{
			assert(s_ActiveProject != nullptr && "Active project is null. Call Project::SetActive() first.");
			return s_ActiveProject->GetResourceDirectory();
		}

		static std::filesystem::path GetResourceRegistryPath()
		{
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().ResourceRegistryPath;
		}

	private:
		ProjectConfig m_Config;

		inline static std::shared_ptr<ResourceManagerBase> s_ResourceManager = nullptr;
		inline static std::shared_ptr<Project> s_ActiveProject = nullptr;
	};
}