#pragma once

namespace Luden
{
	using KeyPressedFunc = void (*)();
	using KeyReleasedFunc = void (*)();

	class ENGINE_API InputAction
	{
	public:
		InputAction() = default;
		~InputAction() = default;
	private:
		KeyPressedFunc m_OnKeyPressed = nullptr;
		KeyReleasedFunc m_OnKeyReleased = nullptr;
	};
}