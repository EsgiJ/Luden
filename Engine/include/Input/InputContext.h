#pragma once

#include "EngineAPI.h"
#include "Input/InputAction.h"
#include "Input/InputMapping.h"

#include <string>
#include <vector>
#include <memory>

namespace Luden
{
	class ENGINE_API InputContext 
	{

	public:
		InputContext(const std::string& name, int priority = 0) : m_Name(name), m_Priority(priority) {}


		const std::string& GetName() const { return m_Name; }
		int GetPriority() const { return m_Priority; }
		void SetPriority(int priority) { m_Priority = priority; }

		bool IsEnabled() const { return m_Enabled; }
		void SetEnabled(bool enabled) { m_Enabled = enabled; }


		// Mapping
		void AddMapping(const InputMapping& mapping);

		void AddChordMapping(const ChordMapping& mapping);

		void AddComboMapping(const ComboMapping& mapping);

		void AddAxisMapping(const AxisMapping& mapping);

		void AddAxis2DMapping(const Axis2DMapping& mapping);

		void ClearMappings();

		void RemoveFromMapping(const InputAction& action);


		// Getters
		const std::vector<InputMapping>& GetMappings() const { return m_InputMappings; }
		const std::vector<ChordMapping>& GetChordMappings() const { return m_ChordMappings; }
		const std::vector<ComboMapping>& GetComboMappings() const { return m_ComboMappings; }
		const std::vector<AxisMapping>& GetAxisMappings() const { return m_AxisMappings; }
		const std::vector<Axis2DMapping>& GetAxis2DMappings() const { return m_Axis2DMappings; }

		bool IsBlocking() const { return m_Blocking; }
		void SetBlocking(bool blocking) { m_Blocking = blocking; }
	private:
		std::string m_Name;
		int m_Priority = 0;
		bool m_Enabled = true;
		bool m_Blocking = false;

		std::vector<InputMapping> m_InputMappings;
		std::vector<ChordMapping> m_ChordMappings;
		std::vector<ComboMapping> m_ComboMappings;
		std::vector<AxisMapping> m_AxisMappings;
		std::vector<Axis2DMapping> m_Axis2DMappings;
	};

	class ENGINE_API InputContextStack
	{
	public:

		void PushContext(std::shared_ptr<InputContext> context);

		void PopContext(const std::string& name);

		void PopContext(std::shared_ptr<InputContext> context);

		void ClearContexts();

		std::shared_ptr<InputContext> FindContext(const std::string& name) const;

		bool IsContextActive(const std::string& name) const;

		std::vector<InputContext*> GetActiveContexts() const;

	private:
		void SortContextsByPriority();
	private:
		std::vector<std::shared_ptr<InputContext>> m_Contexts;
	};
}