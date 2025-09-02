#include "Resource/ResourceRegistry.h"

namespace Luden
{
	const ResourceMetadata& ResourceRegistry::Get(const ResourceHandle& resourceHandle) const
	{
		//TODO: Assert metadata.handle = resourceHandle AND resourceHandle != 0
		return m_ResourceRegistry.at(resourceHandle);
	}

	void ResourceRegistry::Set(const ResourceHandle& resourceHandle, const ResourceMetadata& metadata)
	{
		//TODO: Assert metadata.handle = resourceHandle AND resourceHandle != 0
		m_ResourceRegistry[resourceHandle] = metadata;
	}

	bool ResourceRegistry::Contains(const ResourceHandle& resourceHandle) const
	{
		//TODO: Assert metadata.handle = resourceHandle AND resourceHandle != 0
		return m_ResourceRegistry.find(resourceHandle) != m_ResourceRegistry.end();
	}

	size_t ResourceRegistry::Remove(const ResourceHandle& resourceHandle)
	{
		//TODO Log
		return m_ResourceRegistry.erase(resourceHandle);
	}

	void ResourceRegistry::Clear()
	{
		//TODO Log
		m_ResourceRegistry.clear();
	}
}