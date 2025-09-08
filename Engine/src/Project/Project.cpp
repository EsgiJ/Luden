#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
namespace Luden
{
	void Project::SetActive(std::shared_ptr<Project> project)
	{
		if (s_ActiveProject)
		{
			s_ResourceManager->Shutdown();
			s_ResourceManager = nullptr;
		}
		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			s_ResourceManager = std::make_shared<EditorResourceManager>();
		}
	}

	void Project::SetActiveRuntime(std::shared_ptr<Project> project, std::shared_ptr<ResourcePack> resourcePack)
	{
		if (s_ActiveProject)
		{
			s_ResourceManager->Shutdown();
			s_ResourceManager = nullptr;
		}

		s_ActiveProject = project;

		if (s_ActiveProject)
		{
			s_ResourceManager = std::make_shared<RuntimeResourceManager>();
			std::static_pointer_cast<RuntimeResourceManager>(s_ResourceManager)->SetResourcePack(resourcePack);
		}
	}
}

