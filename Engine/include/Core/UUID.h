#pragma once

#include <memory>

#include "EngineAPI.h"

namespace Luden
{
	class ENGINE_API UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other);

		operator uint64_t() const;
	private:
		uint64_t m_UUID;
	};

	class ENGINE_API UUID32
	{
	public:
		UUID32();
		UUID32(uint32_t uuid);
		UUID32(const UUID32& other);

		operator uint32_t() const;
	private:
		uint32_t m_UUID;
	};
}
namespace std {
	template<>
	struct hash<Luden::UUID>
	{
		std::size_t operator()(const Luden::UUID& uuid) const
		{
			return uuid;
		}
	};

	template<>
	struct hash<Luden::UUID32>
	{
		std::size_t operator()(const Luden::UUID32& uuid) const
		{
			return hash<uint32_t>()((uint32_t)uuid);
		}
	};
}