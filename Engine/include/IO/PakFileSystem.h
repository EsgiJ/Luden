#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <fstream>

#include "EngineAPI.h"
#include "IO/IFileSystem.h"

namespace Luden 
{

	struct ENGINE_API PakEntryInfo {
		uint64_t offset;
		uint64_t size;
	};

	class ENGINE_API PakFile : public IFile
	{
	public:

		PakFile(std::ifstream* pak, uint64_t offset, uint64_t size);

		size_t Read(void* buffer, size_t size) override;
		size_t Write(void* buffer, size_t size) override;
		bool Seek(long offset, int origin) override;
		size_t Tell() override;
		size_t Size() override;
		void Close() override;

	private:
		std::ifstream* m_PakStream;
		uint64_t m_Offset;
		uint64_t m_Size;
		uint64_t m_Position;
	};

	class ENGINE_API PakFileSystem : public IFileSystem
	{
	public:
		PakFileSystem(const std::string& pakPath);
		~PakFileSystem();

		bool LoadPak();
		std::unique_ptr<IFile> Open(const std::string& uuid, FileMode mode) override;
		bool Exists(const std::string& uuid) const override;
		std::vector<std::string> ListFiles(const std::string& directory, const std::string& extension = "") override;
	private:
		std::ifstream m_PakStream;
		std::unordered_map<std::string, PakEntryInfo> m_Index;
		std::string m_PakPath;
	};
}