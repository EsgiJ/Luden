#include "Physics2D/CollisionChannelRegistry.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace Luden
{

	CollisionChannelRegistry::CollisionChannelRegistry()
	{
		RegisterDefaultChannels();
	}

	bool CollisionChannelRegistry::RegisterCollisionChannel(const std::string& name)
	{
		if (HasChannel(name))
			return false;

		if (m_NextAvailableBit >= MAX_CHANNELS)
			return false;

		CollisionChannel newCollisionChannel;
		newCollisionChannel.Name = name;
		newCollisionChannel.Bit = (1u << m_NextAvailableBit);

		m_CollisionChannels.push_back(newCollisionChannel);
		m_NameToIndex[name] = m_CollisionChannels.size() - 1;
		m_NextAvailableBit++;

		return true;
	}

	bool CollisionChannelRegistry::UnregisterCollisionChannel(const std::string& name)
	{
		auto it = m_NameToIndex.find(name);
		if (it == m_NameToIndex.end())
			return false;

		size_t index = it->second;

		m_CollisionChannels.erase(m_CollisionChannels.begin() + index);

		m_NameToIndex.clear();
		for (size_t i = 0; i < m_CollisionChannels.size(); i++)
		{
			m_NameToIndex[m_CollisionChannels[i].Name] = i;
		}

		return true;
	}

	std::string CollisionChannelRegistry::GetChannelName(uint16_t bit) const
	{
		for (const auto& channel : m_CollisionChannels)
		{
			if (channel.Bit == bit)
				return channel.Name;
		}
		return "";
	}

	uint16_t CollisionChannelRegistry::GetChannelBit(const std::string& name) const
	{
		auto it = m_NameToIndex.find(name);
		if (it == m_NameToIndex.end())
			return 0;

		return m_CollisionChannels[it->second].Bit;
	}

	bool CollisionChannelRegistry::HasChannel(const std::string& name) const
	{
		if (m_NameToIndex.find(name) == m_NameToIndex.end())
			return false;
	
		return true;
	}

	const CollisionChannel* CollisionChannelRegistry::GetChannelInfo(const std::string& name) const
	{
		for (auto& channel : m_CollisionChannels)
		{
			if (channel.Name == name)
				return &channel;
		}
		return nullptr;
	}

	uint16_t CollisionChannelRegistry::CreateMask(const std::vector<std::string>& channelNames) const
	{
		uint16_t mask = 0;
		for (const auto& name : channelNames)
		{
			mask |= GetChannelBit(name);
		}

		return mask;
	}

	std::vector<std::string> CollisionChannelRegistry::GetChannelNamesFromMask(uint32_t mask) const
	{
		std::vector<std::string> names;

		for (const auto& channel : m_CollisionChannels)
		{
			if (channel.Bit & mask)
			{
				names.push_back(channel.Name);
			}
		}

		return names;
	}

	void CollisionChannelRegistry::ResetToDefaults()
	{
		Clear();
		RegisterDefaultChannels();
	}

	void CollisionChannelRegistry::Clear()
	{
		m_CollisionChannels.clear();
		m_NameToIndex.clear();
		m_NextAvailableBit = 0;
	}
	bool CollisionChannelRegistry::Serialize(const std::filesystem::path& filepath)
	{
		nlohmann::json outJson;
		std::ofstream out(filepath);

		if (!out.is_open())
			return false;

		nlohmann::json jChannels = nlohmann::json::array();
		
		for (auto& channel : m_CollisionChannels)
		{
			nlohmann::json jChannel;
			jChannel["Name"] = channel.Name;
			jChannel["Bit"] = channel.Bit;

			jChannels.push_back(jChannel);
		}
		outJson["CollisionChannelRegistry"] = jChannels;

		out << outJson.dump(4);

		return true;
	}

	bool CollisionChannelRegistry::Deserialize(const std::filesystem::path& filepath)
	{
		Clear();

		nlohmann::json inJson;
		std::ifstream in(filepath);

		if (!in.is_open())
			return false;

		in >> inJson;

		if (!inJson.contains("CollisionChannelRegistry"))
			return false;


		for (auto& jChannel : inJson["CollisionChannelRegistry"])
		{
			CollisionChannel channel;
			channel.Name = jChannel["Name"];
			channel.Bit= jChannel["Bit"];

			m_CollisionChannels.push_back(channel);
			m_NameToIndex.insert({ channel.Name, channel.Bit });

			uint16_t bitIndex = 0;
			uint16_t temp = channel.Bit;
			while (temp >>= 1) 
				bitIndex++;

			if (bitIndex >= m_NextAvailableBit)
				m_NextAvailableBit = bitIndex + 1;
		}

		return true;
	}

	void CollisionChannelRegistry::RegisterDefaultChannels()
	{
		RegisterCollisionChannel("Default");
		RegisterCollisionChannel("Player");
		RegisterCollisionChannel("Enemy");
		RegisterCollisionChannel("Projectile");
		RegisterCollisionChannel("Ground");
		RegisterCollisionChannel("Wall");
		RegisterCollisionChannel("Trigger");
		RegisterCollisionChannel("Item");
	}
}