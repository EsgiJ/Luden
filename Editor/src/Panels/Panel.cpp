#include "Panels/Panel.h"

namespace Luden::Editor
{
	Panel::Panel(const std::string& name)
		: m_Name(name) {
	}

	const std::string& Panel::GetName() const
	{
		return m_Name;
	}
}
