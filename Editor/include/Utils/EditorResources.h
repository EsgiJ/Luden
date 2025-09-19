#pragma once

#include "Graphics/Texture.h"

#pragma once
#include <filesystem>
#include "Resource/ResourceManager.h"

namespace Luden 
{
	enum class NewProjectType 
	{
		EMPTY,
		//2D Template,
		//3D Template etc.
	};

	class EditorResources 
	{
	public:
		static ResourceHandle Play;
		static ResourceHandle Stop;
		static ResourceHandle Pause;
		static ResourceHandle Step;
		static ResourceHandle Select;
		static ResourceHandle Move;
		static ResourceHandle Scale;
		static ResourceHandle Rotate;
		static ResourceHandle Banner;

		static std::filesystem::path ProjectTemplate;

		static void LoadEditorResources(const std::filesystem::path& engineSourcePath);
	};

}


