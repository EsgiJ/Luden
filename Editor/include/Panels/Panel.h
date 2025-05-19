#pragma once

#include <string>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

#include "EditorAPI.h"

namespace Luden::Editor
{
	class EDITOR_API Panel
	{
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	public:
		Panel(const std::string& name);
		virtual ~Panel() = default;

		virtual void Render() = 0;
		const std::string& GetName() const;

		bool m_Visible = true;
	protected:
		std::string m_Name;
	};
}