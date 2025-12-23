#include "Graphics/Animation.h"
#include "Project/Project.h"

#include <cmath>
#include <utility>


namespace Luden
{
	void Animation::AddFrame(ResourceHandle spriteHandle, float duration)
	{
		AnimationFrame frame;
		frame.spriteHandle = spriteHandle;
		frame.duration = duration;
		m_Frames.push_back(frame);
	}

	void Animation::InsertFrame(size_t index, ResourceHandle spriteHandle, float duration)
	{
		if (index > m_Frames.size())
			index = m_Frames.size();

		AnimationFrame frame;
		frame.spriteHandle = spriteHandle;
		frame.duration = duration;
		m_Frames.insert(m_Frames.begin() + index, frame);
	}

	void Animation::RemoveFrame(size_t index)
	{
		if (index < m_Frames.size())
			m_Frames.erase(m_Frames.begin() + index);
	}

	void Animation::ClearFrames()
	{
		m_Frames.clear();
	}

	std::shared_ptr<Sprite> Animation::GetSprite(size_t index)
	{
		auto sprite = std::static_pointer_cast<Sprite>(Project::GetResourceManager()->GetResource(m_Frames[index].spriteHandle));
		return sprite;
	}

	float Animation::GetTotalDuration() const
	{
		float total = 0.0f;
		for (const auto& frame : m_Frames)
		{
			total += frame.duration;
		}
		return total;
	}
}
