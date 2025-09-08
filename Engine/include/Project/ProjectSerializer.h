#pragma once
#pragma once

#include <filesystem>
#include "EngineAPI.h"
#include "Project/Project.h"

namespace Luden
{
	class ENGINE_API ProjectSerializer
	{
	public:
		ProjectSerializer(std::shared_ptr<Project> project);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

		bool SerializeRuntime(const std::filesystem::path& path);
		bool DeserializeRuntime(const std::filesystem::path& path);


	private:
		const std::shared_ptr<Project> m_Project;
	};
}