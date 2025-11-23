#pragma once

namespace Luden
{
	class InputSystem
	{
	public:

		static InputSystem& Instance()
		{
			static InputSystem inputSystem;
			return inputSystem;
		}

	private:
		InputSystem() = default;

		std::vector<InputComponent>
	};
}