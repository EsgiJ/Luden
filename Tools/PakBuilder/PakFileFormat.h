#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>


namespace Luden
{
	struct PakEntry
	{
		std::string uuid;
		uint64_t offset;
		uint64_t size;
	};

	struct PakIndex
	{
		std::vector<PakEntry> entries;
	};

	constexpr uint32_t PAK_MAGIC = 0x4C50414B; // 'LPAK'
	constexpr uint32_t PAK_VERSION = 1;
}