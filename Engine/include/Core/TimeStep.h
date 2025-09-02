#pragma once

#include "EngineAPI.h"

namespace Luden
{
	class ENGINE_API TimeStep
	{
	public:
		TimeStep() = default;
		TimeStep(float time);

		inline float GetSeconds() const { return m_Time; }
		inline float GetMiliseconds() const { return m_Time * 1000.0f; }

		operator float() { return m_Time; }
	private:
		float m_Time = 0.0f;
	};
}