#pragma once

#include <unordered_map>
#include <string>

#include "EngineAPI.h"

namespace Luden 
{

	class ENGINE_API RuntimeResourceDatabase 
	{
	public:
		bool Load(const std::string& path);
		bool LoadFromJSON(const std::string& path);
		const std::string* GetType(const std::string& uuid) const;
	private:
		std::unordered_map<std::string, std::string> m_Entries;
	};

}
