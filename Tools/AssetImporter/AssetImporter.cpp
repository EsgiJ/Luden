#include <filesystem>
#include <fstream>
#include <iostream>

#include "Resource/ResourceDatabase.h"

namespace fs = std::filesystem;

namespace Luden
{
	int main(int argc, char** argv)
	{
		if (argc < 3)
		{
			std::cout << "Usage: AssetImporter <asset_dir> <output_dir>\n";
			return 1;
		}
		fs::path assetRoot = argv[1];
		fs::path outputRoot = argv[2];

		Luden::ResourceDatabase db;

		for (auto& entry : fs::recursive_directory_iterator(assetRoot))
		{
			if (!entry.is_regular_file())
				continue;

			fs::path source = entry.path();
			std::string ext = source.extension().string();
			std::string type;
			if (ext == ".png") type = "TextureResource";
			else if (ext == ".wav") type = "SoundResource";
			else if (ext == ".ttf") type = "FontResource";
			else continue;

			std::string uuid = source.stem().string();
			fs::path compiled = outputRoot / (uuid + ".bin");

			std::ifstream in(source, std::ios::binary);
			std::ofstream out(compiled, std::ios::binary);
			if (!in.is_open() || !out.is_open())
				continue;
			std::vector<char> buffer(std::istreambuf_iterator<char>(in), {});
			out.write(buffer.data(), buffer.size());

			Luden::ResourceMeta meta;
			meta.uuid = uuid;
			meta.type = type;
			meta.sourcePath = source.string();
			meta.compiledPath = compiled.string();
			db.AddOrUpdate(meta);
		}

		db.SaveToFile("resources.json");
		return 0;
	}
}

