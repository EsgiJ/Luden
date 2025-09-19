#pragma once

#include "IO/FileSystem.h"
#include "Resource/Resource.h"
#include "Resource/ResourceMetadata.h"
#include "Resource/ResourceManager.h"
#include "Utils/FileMetadata.h"
#include "Project/Project.h"

#include <string_view>

namespace Luden
{
	namespace ImGuiUtils
	{
		void PrefixLabel(std::string_view title);

		bool ResourceButton(ResourceHandle& handle, ResourceType resourceType);
	}
}