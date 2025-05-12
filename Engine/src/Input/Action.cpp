#include "Input/Action.h"

#include <sstream>

namespace Luden {

	Action::Action(std::string name, std::string type)
		: m_Name(std::move(name)), m_Type(std::move(type)) {
	}

	Action::Action(std::string name, Math::Vec2 pos)
		: m_Name(std::move(name)), m_Pos(pos) {
	}

	Action::Action(std::string name, std::string type, Math::Vec2 pos)
		: m_Name(std::move(name)), m_Type(std::move(type)), m_Pos(pos) {
	}

	const std::string& Action::Name() const {
		return m_Name;
	}

	const std::string& Action::Type() const {
		return m_Type;
	}

	const Math::Vec2& Action::Pos() const {
		return m_Pos;
	}

	std::string Action::ToString() const {
		std::stringstream ss;
		ss << m_Name << " " << m_Type << " "
			<< static_cast<int>(m_Pos.x) << " "
			<< static_cast<int>(m_Pos.y);
		return ss.str();
	}

}
