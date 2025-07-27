#include "Resource/ResourceDatabase.h"
#include "PakFileFormat.h"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace Luden {
	int main()
	{
		ResourceDatabase db;
		if (!db.LoadFromFile("resources.json"))
		{
			printf("Could not load resources.json\n");
			return 1;
		}

		std::ofstream pak("Build/game.pak", std::ios::binary);
		if (!pak.is_open())
		{
			printf("Can not open game.pak\n");
			return 1;
		}

		constexpr const char MAGIC[4] = { 'L', 'P', 'A', 'K' };
		pak.write(MAGIC, 4);
		uint32_t version = PAK_VERSION;

		pak.write(reinterpret_cast<char*>(&version), sizeof(uint32_t));

		uint64_t indexOffset = 0;
		pak.write(reinterpret_cast<char*>(&indexOffset), sizeof(uint64_t));

		PakIndex index;
		for (const auto& [uuid, meta] : db.GetAll())
		{
			std::ifstream res(meta.compiledPath, std::ios::binary | std::ios::ate);
			if (!res.is_open())
				continue;

			size_t size = res.tellg();
			res.seekg(0);

			PakEntry entry;
			entry.uuid = uuid;
			entry.offset = static_cast<uint64_t>(pak.tellp());
			entry.size = size;

			std::vector<char> buffer(size);
			res.read(buffer.data(), size);
			pak.write(buffer.data(), size);

			index.entries.push_back(entry);
		}

		// Index offset
		indexOffset = static_cast<uint64_t>(pak.tellp());

		uint32_t entryCount = static_cast<uint32_t>(index.entries.size());
		pak.write(reinterpret_cast<char*>(&entryCount), sizeof(uint32_t));

		for (auto& entry : index.entries)
		{
			uint16_t uuidLen = static_cast<uint16_t>(entry.uuid.size());
			pak.write(reinterpret_cast<char*>(&uuidLen), sizeof(uint16_t));
			pak.write(reinterpret_cast<char*>(&entry.uuid), uuidLen);
			pak.write(reinterpret_cast<char*>(&entry.offset), sizeof(uint64_t));
			pak.write(reinterpret_cast<char*>(&entry.size), sizeof(uint64_t));
		}

		// Rewrite index offset at beginning
		pak.seekp(0);
		pak.write(reinterpret_cast<char*>(&indexOffset), sizeof(uint64_t));
		pak.close();

		printf("Build/game.pak generated successfully.\n");
		return 0;
	}
}
