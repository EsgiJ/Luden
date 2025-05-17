#pragma once

#include <string>

#include <rttr/registration_friend.h>

#include "EngineAPI.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden {

	class ENGINE_API Action 
	{
	private:
		RTTR_REGISTRATION_FRIEND

		std::string m_Name;
		std::string m_Type;
		Math::Vec2 m_Pos = { 0.0f, 0.0f };

	public:
		Action() = default;
		Action(std::string name, std::string type);
		Action(std::string name, Math::Vec2 pos);
		Action(std::string name, std::string type, Math::Vec2 pos);

		[[nodiscard]] const std::string& Name() const;
		[[nodiscard]] const std::string& Type() const;
		[[nodiscard]] const Math::Vec2& Pos() const;

		[[nodiscard]] std::string ToString() const;
	};
}
