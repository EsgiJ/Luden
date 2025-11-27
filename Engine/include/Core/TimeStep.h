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

		operator float() const { return m_Time; }

		TimeStep& operator +=(const TimeStep& other)
		{
			m_Time += other.GetSeconds(); 
			return *this;
		}

		friend TimeStep operator +(const TimeStep& a, const TimeStep& b)
		{ 
			return a.GetSeconds() + b.GetSeconds(); 

		}

	private:
		float m_Time = 0.0f;
	};
}