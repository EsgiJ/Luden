#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include "Graphics/Texture.h"

#include <SFML/Graphics/Texture.hpp>
#include <glm/vec2.hpp>

namespace Luden
{
	struct ENGINE_API AnimationFrame
	{
		ResourceHandle textureHandle = 0;  
		float duration = 1.0f;             
		glm::vec2 offset = { 0, 0 };        
	};

	class ENGINE_API Animation : public Resource
	{
	public:
		Animation() = default;
		~Animation() = default;

		void AddFrame(ResourceHandle texture, float duration = 1.0f);
		void InsertFrame(size_t index, ResourceHandle texture, float duration = 1.0f);
		void RemoveFrame(size_t index);
		void ClearFrames();

		size_t GetFrameCount() const { return m_Frames.size(); }
		const AnimationFrame& GetFrame(size_t index) const { return m_Frames[index]; }
		AnimationFrame& GetFrame(size_t index) { return m_Frames[index]; }
		const std::vector<AnimationFrame>& GetFrames() const { return m_Frames; }

		const std::string& GetName() { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		float GetTotalDuration() const;
		bool IsLooping() const { return m_Loop; }
		void SetLooping(bool loop) { m_Loop = loop; }

		static ResourceType GetStaticType() { return ResourceType::Animation; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

	private:
		std::vector<AnimationFrame> m_Frames;
		bool m_Loop = true;
		std::string m_Name;

		friend class AnimationSerializer;
	};
}
