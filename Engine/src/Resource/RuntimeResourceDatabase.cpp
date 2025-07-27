#include "Resource/RuntimeResourceDatabase.h"

#include <fstream>

namespace Luden {
	bool RuntimeResourceDatabase::Load(const std::string& path)
	{
		std::ifstream in(path, std::ios::binary);
		if (!in.is_open())
			return false;

		uint32_t count;
		in.read(reinterpret_cast<char*>(&count), 4);
		for (uint32_t i = 0; i < count; i++)
		{
			uint16_t uuidLen;
			uint16_t typeLen;
			in.read(reinterpret_cast<char*>(&uuidLen), sizeof(uint16_t));
			std::string uuid(uuidLen, ' ');
			in.read(uuid.data(), uuidLen);

			in.read(reinterpret_cast<char*>(&typeLen), sizeof(uint16_t));
			std::string type(typeLen, ' ');
			in.read(type.data(), typeLen);

			m_Entries[uuid] = type;
		}

		return true;
	}
	const std::string* RuntimeResourceDatabase::GetType(const std::string& uuid) const
	{
		auto it = m_Entries.find(uuid);
		if (it == m_Entries.end())
			return nullptr;

		return &it->second;
	}
}
