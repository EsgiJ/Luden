#pragma once

#include <string>

#include "EngineAPI.h"

namespace Luden
{
//TODO: this doesn't belong here, later on create Base.h and put it there
#ifndef BIT
#define BIT(x) static_cast<uint16_t>(1u << (x))
#endif

	enum class ENGINE_API ResourceFlag : uint16_t
	{
		None = 0,
		Missing = BIT(0),
		Invalid = BIT(1)
	};

	enum class ENGINE_API ResourceType : uint16_t
	{
		None = 0,
		Scene,
		Prefab,
		Texture,
		Sprite,
		Sound,
		Music,
		Font,
		Animation,
		NativeScript
	};

	namespace Utils
	{
		inline ResourceType ResourceTypeToString(std::string_view resourceType)
		{
			if (resourceType == "None")				return ResourceType::None;
			if (resourceType == "Scene")			return ResourceType::Scene;
			if (resourceType == "Prefab")			return ResourceType::Prefab;
			if (resourceType == "Texture")			return ResourceType::Texture;
			if (resourceType == "Sprite")			return ResourceType::Sprite;
			if (resourceType == "Sound")			return ResourceType::Sound;
			if (resourceType == "Music")			return ResourceType::Music;
			if (resourceType == "Font")				return ResourceType::Font;
			if (resourceType == "Animation")		return ResourceType::Animation;
			if (resourceType == "NativeScript")		return ResourceType::NativeScript;

			return ResourceType::None;
		}

		inline const char* ResourceTypeToString(const ResourceType& resourceType)
		{
			switch (resourceType)
			{
			case ResourceType::None:			return "None";
			case ResourceType::Scene:			return "Scene";
			case ResourceType::Prefab:			return "Prefab";
			case ResourceType::Texture:			return "Texture";
			case ResourceType::Sprite:			return "Sprite";
			case ResourceType::Sound:			return "Sound";
			case ResourceType::Music:			return "Music";
			case ResourceType::Font:			return "Font";
			case ResourceType::Animation:		return "Animation";
			case ResourceType::NativeScript:	return "NativeScript";
			}

			return "None";
		}
	}
}
