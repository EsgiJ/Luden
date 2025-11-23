#pragma once

namespace Luden
{
	class ENGINE_API InputAction : public Resource
	{
	public:
		InputAction() = default;
		explicit InputAction(const std::string& name)
			: m_Name(name)
		{ }
	private:
		std::string m_Name;
	};
}