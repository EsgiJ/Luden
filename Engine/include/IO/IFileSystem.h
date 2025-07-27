#pragma once

#include <memory>
#include <string>
#include <vector>

#include "EngineAPI.h"


namespace Luden 
{
	enum class ENGINE_API FileMode
	{
		Read,
		Write,
		ReadWrite
	};

	class ENGINE_API IFile
	{
	public:

		virtual ~IFile() = default;
		virtual size_t Read(void* buffer, size_t size) = 0;
		virtual size_t Write(void* buffer, size_t size) = 0;
		virtual bool Seek(long offset, int origin) = 0;
		virtual size_t Tell() = 0;
		virtual size_t Size() = 0;
		virtual void Close() = 0;
	};

	class ENGINE_API IFileSystem
	{
	public:

		virtual std::unique_ptr<IFile> Open(const std::string& path, FileMode mode) = 0;
		virtual bool Exists(const std::string& path) const = 0;
		virtual std::vector<std::string> ListFiles(const std::string& directory, const std::string& extension) const = 0;
	};
}