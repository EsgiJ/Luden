#pragma once

#include "Resource/Resource.h"
#include "EngineAPI.h"

#include <filesystem>
#include <memory>
#include <string>

namespace Luden
{
	enum class ENGINE_API ResourceStatus
	{
		None = 0,
		Ready = 1,
		Invalid = 2,
		Loading = 3
	};

	struct ResourceMetadata
	{
		ResourceHandle Handle = 0;
		ResourceType Type = ResourceType::None;
		std::filesystem::path FilePath;

		ResourceStatus Status = ResourceStatus::None;

		bool IsDataLoaded = false;

		bool IsValid() const { return Handle != 0; }
	};

	struct EditorResourceLoadResponse
	{
		ResourceMetadata Metadata;
		std::shared_ptr<Resource> Resource;
	};


	struct RuntimeResourceLoadRequest
	{
		ResourceHandle SceneHandle = 0;
		ResourceHandle Handle = 0;
	};

}
