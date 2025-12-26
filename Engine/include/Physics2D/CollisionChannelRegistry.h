#pragma once

#include "EngineAPI.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace Luden
{
	struct ENGINE_API CollisionChannel
	{
		std::string Name;
		uint16_t Bit;
	};

	class ENGINE_API CollisionChannelRegistry
	{
	public:

		static CollisionChannelRegistry& Instance()
		{
			static CollisionChannelRegistry s_Instance;
			return s_Instance;
		}

		CollisionChannelRegistry(const CollisionChannelRegistry&) = delete;
		CollisionChannelRegistry& operator=(const CollisionChannelRegistry&) = delete;

		bool RegisterCollisionChannel(const std::string& name);
		bool UnregisterCollisionChannel(const std::string& name);

		std::string GetChannelName(uint16_t bit) const;
		uint16_t GetChannelBit(const std::string& name) const;

		bool HasChannel(const std::string& name) const;

		const std::vector<CollisionChannel>& GetAllChannels() const { return m_CollisionChannels; }

		const CollisionChannel* GetChannelInfo(const std::string& name) const;

		uint16_t CreateMask(const std::vector<std::string>& channelNames) const;

		std::vector<std::string> GetChannelNamesFromMask(uint32_t mask) const;

		void ResetToDefaults();

		void Clear();
	
		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		CollisionChannelRegistry();

		void RegisterDefaultChannels();
	private:
		std::vector<CollisionChannel> m_CollisionChannels;
		std::unordered_map<std::string, size_t> m_NameToIndex;
		uint16_t m_NextAvailableBit = 0;
		static constexpr uint16_t MAX_CHANNELS = 16;

	};
}
