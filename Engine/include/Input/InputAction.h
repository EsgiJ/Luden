#pragma once

#include "Resource/Resource.h"

namespace Luden
{
	class ENGINE_API InputAction : public Resource
	{
	public:
		InputAction() = default;
		explicit InputAction(const std::string& name)
			: m_Name(name)
		{ }

		//Operators
		bool operator ==(const InputAction& other) const { return Handle == other.Handle; }
		bool operator !=(const InputAction& other) const { return Handle != other.Handle; }

	private:
		std::string m_Name;
	};
}