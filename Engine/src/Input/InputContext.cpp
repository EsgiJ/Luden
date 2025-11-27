#include "Input/InputContext.h"

#include <algorithm>

namespace Luden
{
	// InputContext 
	void InputContext::AddMapping(const InputMapping& mapping)
	{
		m_InputMappings.push_back(mapping);
	}

	void InputContext::AddChordMapping(const ChordMapping& mapping)
	{
		m_ChordMappings.push_back(mapping);
	}

	void InputContext::AddComboMapping(const ComboMapping& mapping)
	{
		m_ComboMappings.push_back(mapping);
	}

	void InputContext::AddAxisMapping(const AxisMapping& mapping)
	{
		m_AxisMappings.push_back(mapping);
	}

	void InputContext::AddAxis2DMapping(const Axis2DMapping& mapping)
	{
		m_Axis2DMappings.push_back(mapping);
	}

	void InputContext::ClearMappings()
	{
		m_InputMappings.clear();
		m_ChordMappings.clear();
		m_ComboMappings.clear();
		m_AxisMappings.clear();
		m_Axis2DMappings.clear();
	}

	void InputContext::RemoveFromMapping(const InputAction& action)
	{
		auto removeFromVector = [&action](auto& vec)
			{
				vec.erase
				(
					std::remove_if
					(
						vec.begin(),
						vec.end(),
						[&action](auto& mapping){ return mapping.action == action; }
					),
					vec.end()
				);
			};
	}

	void InputContextStack::PushContext(std::shared_ptr<InputContext> context)
	{
		m_Contexts.push_back(context);
		
		SortContextsByPriority();
	}

	void InputContextStack::PopContext(const std::string& name)
	{
		m_Contexts.erase
		(
			std::remove_if
			(
				m_Contexts.begin(),
				m_Contexts.end(),
				[&name](const auto& ctx) { return ctx->GetName() == name; }
			),
			m_Contexts.end()
		);
	}

	void InputContextStack::PopContext(std::shared_ptr<InputContext> context)
	{
		m_Contexts.erase
		(
			std::remove(m_Contexts.begin(), m_Contexts.end(), context),
			m_Contexts.end()
		);
	}

	void InputContextStack::ClearContexts()
	{
		m_Contexts.clear();
	}

	std::shared_ptr<InputContext> InputContextStack::FindContext(const std::string& name) const
	{
		auto it = std::find_if
		(
			m_Contexts.begin(),
			m_Contexts.end(),
			[&name](const auto& ctx) { return ctx->GetName() == name; }
		);
		return it != m_Contexts.end() ? *it : nullptr;
	}

	bool InputContextStack::IsContextActive(const std::string& name) const
	{
		auto ctx = FindContext(name);
		return ctx && ctx->IsEnabled();
	}

	std::vector<InputContext*> InputContextStack::GetActiveContexts() const
	{
		std::vector<InputContext*> activeContexts;

		for (const auto& ctx: m_Contexts)
		{
			if (ctx->IsEnabled())
			{
				activeContexts.push_back(ctx.get());
			}
		}
		return activeContexts;
	}

	void InputContextStack::SortContextsByPriority()
	{
		std::sort
		(
			m_Contexts.begin(),
			m_Contexts.end(),
			[](const auto& a, const auto& b) { return a->GetPriority() > b->GetPriority(); }
		);
	}
}