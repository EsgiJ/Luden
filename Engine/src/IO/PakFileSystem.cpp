#include <iostream>

#include "IO/PakFileSystem.h"
#include "PakBuilder/PakFileFormat.h"

namespace fs = std::filesystem;

namespace Luden
{
	// PakFile
	PakFile::PakFile(std::ifstream* pak, uint64_t offset, uint64_t size)
		: m_PakStream(pak), m_Offset(offset), m_Size(size), m_Position(0) {}

	size_t PakFile::Read(void* buffer, size_t size)
	{
		// Prevent exceeding end of file
		if (m_Position + size > m_Size)
		{
			size = m_Size - m_Position; 
		}

		m_PakStream->seekg(m_Offset + m_Position);
		m_PakStream->read(reinterpret_cast<char*>(buffer), size);
		m_Position += m_PakStream->gcount();
		return m_PakStream->gcount();
	}
	size_t PakFile::Write(void* buffer, size_t size)
	{
		// Not supported
		return 0;
	}
	bool PakFile::Seek(long offset, int origin)
	{
		uint64_t newPos;

		if (origin == SEEK_SET)
			newPos = offset;
		else if (origin == SEEK_CUR)
			newPos = m_Position + offset;
		else if (origin == SEEK_END)
			newPos = m_Size + offset;
		else
			return false;

		if (newPos > m_Size)
			return false;

		m_Position = newPos;
		return true;
	}

	size_t PakFile::Tell()
	{
		return m_Position;
	}

	size_t PakFile::Size()
	{
		return static_cast<size_t>(m_Size);
	}

	void PakFile::Close()
	{
		//No-op
	}

	// PakFileSystem
	PakFileSystem::PakFileSystem(const std::string& pakPath)
		: m_PakPath(pakPath)
	{
		LoadPak();
	}

	PakFileSystem::~PakFileSystem()
	{
		if (m_PakStream.is_open())
			m_PakStream.close();
	}

	bool PakFileSystem::LoadPak()
	{
		m_PakStream.open(m_PakPath, std::ios::binary);
		if (!m_PakStream.is_open())
			return false;

		char magic[4];
		m_PakStream.read(magic, 4);
		if (memcmp(magic, "LPAK", 4) != 0)
			return false;

		uint32_t version;
		m_PakStream.read(reinterpret_cast<char*>(&version), 4);
		if (version != PAK_VERSION)
			return false;

		uint64_t indexOffset;
		m_PakStream.read(reinterpret_cast<char*>(&indexOffset), 8);
		m_PakStream.seekg(indexOffset);

		uint32_t count;
		m_PakStream.read(reinterpret_cast<char*>(&count), 4);
		for (uint32_t i = 0; i < count; i++)
		{
			uint16_t uuidLen;
			m_PakStream.read(reinterpret_cast<char*>(&uuidLen), 2);
			std::string uuid(uuidLen, ' ');
			m_PakStream.read(uuid.data(), uuidLen);

			uint64_t offset;
			uint64_t size;
			m_PakStream.read(reinterpret_cast<char*>(&offset), 8);
			m_PakStream.read(reinterpret_cast<char*>(&size), 8);

			m_Index[uuid] = { offset, size };
		}

		return true;
	}

	std::unique_ptr<IFile> PakFileSystem::Open(const std::string& uuid, FileMode mode)
	{
		if (mode != FileMode::Read)
			return nullptr;
		auto it = m_Index.find(uuid);

		if (it == m_Index.end())
			return nullptr;

		return std::make_unique<PakFile>(&m_PakStream, it->second.offset, it->second.size);
	}

	bool PakFileSystem::Exists(const std::string& uuid) const
	{
		return m_Index.find(uuid) != m_Index.end();
	}

	std::vector<std::string> PakFileSystem::ListFiles(const std::string& directory, const std::string& extension)
	{
		std::vector<std::string> keys;
		for (const auto& [uuid, _] : m_Index)
			keys.push_back(uuid);
		return keys;
	}

}