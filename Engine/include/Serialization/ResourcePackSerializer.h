#pragma once

#include "Serialization/ResourcePackFile.h"
#include "Core/Buffer.h"

#include <filesystem>

namespace Luden {

	class ResourcePackSerializer
	{
	public:
		static void Serialize(const std::filesystem::path& path, ResourcePackFile& file, Buffer appBinary, std::atomic<float>& progress);
		static bool DeserializeIndex(const std::filesystem::path& path, ResourcePackFile& file);
	private:
		static uint64_t CalculateIndexTableSize(const ResourcePackFile& file);
	};

}
