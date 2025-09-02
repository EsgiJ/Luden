#pragma once

#include <unordered_map>

#include "ResourceTypes.h"

namespace Luden
{
	inline static std::unordered_map<std::string, ResourceType> s_ResourceExtensionMap =
	{
		// Luden types
		{".lscene", ResourceType::Scene},
		{".lanim", ResourceType::Animation},
		{".lprefab", ResourceType::Prefab},
		{".lscene", ResourceType::Scene},

		//Textures
		{".png", ResourceType::Texture},
		{".jpg", ResourceType::Texture},
		{".jpeg", ResourceType::Texture},

		//Audio
		{".wav", ResourceType::Audio},
		{".ogg", ResourceType::Audio},

		//Fonts
		{".ttf", ResourceType::Font},
		{".ttc", ResourceType::Font},
		{".otf", ResourceType::Font},
	};
}