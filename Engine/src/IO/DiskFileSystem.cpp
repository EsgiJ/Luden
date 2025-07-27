#include <filesystem>
#include <fstream>

#include "IO/DiskFileSystem.h"

namespace fs = std::filesystem;

namespace Luden
{
	// DiskFile
	DiskFile::DiskFile(std::fstream&& stream)
		: m_Stream(std::move(stream)) {}

	size_t DiskFile::Read(void* buffer, size_t size)
	{
		m_Stream.read(reinterpret_cast<char*>(buffer), size);
		return m_Stream.gcount();
	}

	size_t DiskFile::Write(void* buffer, size_t size)
	{
		m_Stream.write(reinterpret_cast<char*>(buffer), size);
		return size;
	}

	bool DiskFile::Seek(long offset, int origin)
	{
		std::ios_base::seekdir dir;

		if (origin == SEEK_SET)
			dir = std::ios::beg;

		else if (origin == SEEK_CUR)
			dir = std::ios::cur;

		else if (origin == SEEK_END)
			dir = std::ios::end;

		else
			return false;

		m_Stream.seekg(offset, dir);
		m_Stream.seekp(offset, dir);
		return true;
	}

	size_t DiskFile::Tell()
	{
		return static_cast<size_t>(m_Stream.tellg());
	}

	size_t DiskFile::Size()
	{
		std::streampos current = m_Stream.tellg();
		m_Stream.seekg(0, std::ios::end);
		size_t size = static_cast<size_t>(m_Stream.tellg());
		m_Stream.seekg(current);
		return size;
	}

	void DiskFile::Close()
	{
		m_Stream.close();
	}

	// DiskFileSystem
	std::unique_ptr<IFile> DiskFileSystem::Open(const std::string& path, FileMode mode)
	{
		std::ios::openmode openMode = std::ios::binary;

		if (mode == FileMode::Read)
			openMode |= std::ios::in;

		else if (mode == FileMode::Write)
			openMode |= std::ios::out | std::ios::trunc;

		else if (mode == FileMode::ReadWrite)
			openMode |= std::ios::in | std::ios::out;

		std::fstream file(path, openMode);

		if (!file.is_open())
			return nullptr;

		return std::make_unique<DiskFile>(std::move(file));
	}

	bool DiskFileSystem::Exists(const std::string& path) const
	{
		return fs::exists(path);
	}

	std::vector<std::string> DiskFileSystem::ListFiles(const std::string& directory, const std::string& extension) const
	{
		std::vector<std::string> results;
		for (const auto& entry: fs::recursive_directory_iterator(directory))
		{
			if (!fs::is_regular_file(entry))
				continue;
			
			if (!extension.empty() && entry.path().extension() != extension) 
				continue;

			results.push_back(entry.path().string());
		}
		return results;
	}
}