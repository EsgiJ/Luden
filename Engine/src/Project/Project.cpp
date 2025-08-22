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

/*	void Project::SetActiveRuntime(std::shared_ptr<Project> project, std::shared_ptr<ResourcePack> resourcePack)
	{
		//TODO:
	}
*/
	std::shared_ptr<Project> Project::New()
	{
		s_ActiveProject = std::make_shared<Project>();
		return s_ActiveProject;
	}
	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		std::shared_ptr<Project> project = std::make_shared<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->GetConfig().ProjectDirectory = path.parent_path().string();
			s_ActiveProject = project;
			return s_ActiveProject;
		}
		return nullptr;
	}
	bool Project::SaveActiveProject(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->GetConfig().ProjectDirectory = path.parent_path().string();
			return true;
		}
		return false;
	}

}

