#pragma once

#include "NativeScript/ScriptableEntity.h"
#include "Scene/Prefab.h"
#include "Graphics/Animation.h"
#include "Graphics/Sprite.h"
#include "Graphics/Font.h"
#include "Audio/Sound.h"
#include "Audio/Music.h"
#include "Resource/Resource.h"

#include <glm/fwd.hpp>

namespace Luden
{
	// Resources
	using PrefabRef = std::shared_ptr<Prefab>;
	using AnimationRef = std::shared_ptr<Animation>;
	using SoundRef = std::shared_ptr<Sound>;
	using MusicRef = std::shared_ptr<Music>;
	using SpriteRef = std::shared_ptr<Sprite>;

	// Pointer 
	template<typename T>
	using Ref = std::shared_ptr<T>;

	// Data Structures
	template<typename T>
	using Vector = std::vector<T>;

	template<typename Key, typename Value>
	using HashMap = std::unordered_map<Key, Value>;

	using String = std::string;

	// Glm
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;

	using IVec2 = glm::ivec2;  
	using IVec3 = glm::ivec3;

	using Color = glm::vec4;
}
