#include "Utils/EditorResources.h"
#include "IO/FileSystem.h"
#include "Project/Project.h"

namespace Luden {

	ResourceHandle EditorResources::Play;
	ResourceHandle EditorResources::Stop;
	ResourceHandle EditorResources::Pause;
	ResourceHandle EditorResources::Step;
	ResourceHandle EditorResources::Select;
	ResourceHandle EditorResources::Move;
	ResourceHandle EditorResources::Scale;
	ResourceHandle EditorResources::Rotate;
	ResourceHandle EditorResources::Banner;

	std::filesystem::path EditorResources::ProjectTemplate;

	static inline ResourceHandle Load(const std::filesystem::path& path) {
		auto absolute = std::filesystem::absolute(path);
		if (!FileSystem::Exists(absolute))
			return 0; 
		return Project::GetEditorResourceManager()->ImportResource(absolute);
	}

	void EditorResources::LoadEditorResources(const std::filesystem::path& engineSourcePath) {
		std::filesystem::path resources = engineSourcePath / "Editor/Resources";

		Play = Load(resources / "icons/play_button.png");
		Stop = Load(resources / "icons/stop_button.png");
		Pause = Load(resources / "icons/pause_button.png");
		Step = Load(resources / "icons/step_button.png");

		Select = Load(resources / "icons/tool_select.png");
		Move = Load(resources / "icons/tool_move.png");
		Scale = Load(resources / "icons/tool_scale.png");
		Rotate = Load(resources / "icons/tool_rotate.png");

		Banner = Load(resources / "icons/banner.png");

		ProjectTemplate = engineSourcePath / "project";
	}

}
