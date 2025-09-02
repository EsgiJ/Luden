#include "EngineAPI.h"
#include "Resource/ResourceMetadata.h"


#include <unordered_map>

namespace Luden
{
	class ResourceRegistry
	{
	public:
		const ResourceMetadata& Get(const ResourceHandle& resourceHandle) const;
		void Set(const ResourceHandle& resourceHandle, const ResourceMetadata& metadata);

		size_t Count() const { return m_ResourceRegistry.size(); }
		bool Contains(const ResourceHandle& resourceHandle) const;
		size_t Remove(const ResourceHandle& resourceHandle);
		void Clear();

		auto begin() { return m_ResourceRegistry.begin(); }
		auto end() { return m_ResourceRegistry.end(); }
		auto begin() const { return m_ResourceRegistry.cbegin(); }
		auto end() const { return m_ResourceRegistry.cend(); }
	private:
		std::unordered_map<ResourceHandle, ResourceMetadata> m_ResourceRegistry;
	};
}
