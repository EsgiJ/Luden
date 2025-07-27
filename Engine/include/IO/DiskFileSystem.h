#pragma once

#include <fstream>

#include "EngineAPI.h"
#include "IO/IFileSystem.h"

namespace Luden 
{
	class ENGINE_API DiskFile : public IFile
	{
	public:

		DiskFile(std::fstream&& stream);

		size_t Read(void* buffer, size_t size) override;
		size_t Write(void* buffer, size_t size) override;
		bool Seek(long offset, int origin) override;
		size_t Tell() override;
		size_t Size() override;
		void Close() override;

	private:
		std::fstream m_Stream;
	};

	class ENGINE_API DiskFileSystem : public IFileSystem
	{
		virtual std::unique_ptr<IFile> Open(const std::string& path, FileMode mode) override;
		virtual bool Exists(const std::string& path) const override;
		virtual std::vector<std::string> ListFiles(const std::string& directory, const std::string& extension = "") const override;
	};
}