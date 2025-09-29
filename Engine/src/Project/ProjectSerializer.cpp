#include "Project/ProjectSerializer.h"

#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;
namespace Luden
{
	ProjectSerializer::ProjectSerializer(std::shared_ptr<Project> project)
		: m_Project(project)
	{
	}
	bool ProjectSerializer::Serialize(const std::filesystem::path& path)
	{
		const auto& config = m_Project->GetConfig();

		json jProject;

		jProject["Name"] = config.Name;
		jProject["ProjectFileName"] = config.ProjectFileName;
		jProject["ProjectDirectory"] = config.ProjectDirectory;
		jProject["ResourceDirectory"] = config.ResourceDirectory;
		jProject["ResourceRegistry"] = config.ResourceRegistryPath;
		jProject["StartScene"] = config.StartScene.string();

		std::ofstream out(path);
		if (!out.is_open())
			return false;

		out << jProject.dump(4);
		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& path)
	{
		std::ifstream in(path);
		if (!in.is_open())
			return false;

		json jProject;
		in >> jProject;

		auto& config = m_Project->GetConfig();

		if (jProject.contains("Name"))
			config.Name = jProject["Name"].get<std::string>();

		if (jProject.contains("ProjectFileName"))
			config.ProjectFileName = jProject["ProjectFileName"].get<std::string>();

		if (jProject.contains("ProjectDirectory"))
			config.ProjectDirectory = jProject["ProjectDirectory"].get<std::string>();

		if (jProject.contains("ResourceDirectory"))
			config.ResourceDirectory = jProject["ResourceDirectory"].get<std::string>();

		if (jProject.contains("ResourceRegistry"))
			config.ResourceRegistryPath = jProject["ResourceRegistry"].get<std::string>();

		if (jProject.contains("StartScene"))
			config.StartScene = jProject["StartScene"].get<std::string>();

		return true;
	}

	bool ProjectSerializer::SerializeRuntime(const std::filesystem::path& path) {
		const auto& config = m_Project->GetConfig();

		json jRuntime;
		jRuntime["Name"] = config.Name;
		jRuntime["StartSceneHandle"] = static_cast<uint64_t>(config.StartSceneHandle);

		std::ofstream out(path);
		if (!out.is_open())
			return false;

		out << jRuntime.dump();
		return true;
	}

	bool ProjectSerializer::DeserializeRuntime(const std::filesystem::path& path) {
		std::ifstream in(path);
		if (!in.is_open())
			return false;

		json jRuntime;
		in >> jRuntime;

		auto& config = m_Project->GetConfig();

		if (jRuntime.contains("Name"))
			config.Name = jRuntime["Name"].get<std::string>();

		if (jRuntime.contains("StartSceneHandle"))
			config.StartSceneHandle = jRuntime["StartSceneHandle"].get<std::uint64_t>();

		config.ProjectFileName = path.filename().string();
		config.ProjectDirectory = path.parent_path().string();

		return true;
	}

}

