#include "Graphics/Animation.h"
#include "Project/Project.h"

#include <cmath>
#include <utility>


namespace Luden
{
	void Animation::AddFrame(ResourceHandle texture, float duration)
	{
		AnimationFrame frame;
		frame.textureHandle = texture;
		frame.duration = duration;
		m_Frames.push_back(frame);
	}

	void Animation::InsertFrame(size_t index, ResourceHandle texture, float duration)
	{
		if (index > m_Frames.size())
			index = m_Frames.size();

		AnimationFrame frame;
		frame.textureHandle = texture;
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
