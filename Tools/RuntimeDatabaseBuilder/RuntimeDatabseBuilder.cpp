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

		std::ofstream out("Build/resources.runtimedb", std::ios::binary);
		if (!out.is_open())
		{
			printf("Can not open resources.runtimedb\n");
			return 1;
		}

		uint32_t count = static_cast<uint32_t>(db.GetAll().size());
		out.write(reinterpret_cast<char*>(&count), 4);

		for (const auto& [uuid, meta] : db.GetAll())
		{
			uint16_t uuidLen = uuid.size();
			uint16_t typeLen = meta.type.size();

			out.write(reinterpret_cast<char*>(&uuidLen), sizeof(uint16_t));
			out.write(uuid.data(), uuidLen);

			out.write(reinterpret_cast<char*>(typeLen), sizeof(uint16_t));
			out.write((meta.type.data()), typeLen);
		}
		return 0;
	}
}
