#pragma once

#include "Input/InputTypes.h"
#include "Input/InputMapping.h"
#include "Input/InputContext.h"
#include "EngineAPI.h"

#include <SFML/Window/Event.hpp>

namespace Luden
{
	class EntityManager;
	class Entity;
	class TimeStep;

	struct ENGINE_API KeyState
	{
		bool pressed = false;
		bool wasPressed = false;
		float pressedTime = 0.0f;
		float releasedTime = 0.0f;
		float lastPulseTime = 0.0f;
		InputValue currentValue;
	};

	class ENGINE_API InputManager
	{
	public:

		static InputManager& Instance()
		{
			static InputManager s_Instance;
			return s_Instance;
		}

		InputManager(const InputManager&) = delete;
		InputManager& operator =(const InputManager&) = delete;

		void PushContext(std::shared_ptr<InputContext> context);
		void PopContext(const std::string& name);
		void PopContext(std::shared_ptr<InputContext> context);
		std::shared_ptr<InputContext> FindContext(const std::string& name) const;
		InputContextStack& GetContextStack() { return m_ContextStack; }

		void ProcessEvent(const sf::Event& evt, EntityManager& entityManager);
		void Update(TimeStep ts, EntityManager& entityManager);

		bool IsKeyDown(const InputKey& key) const;
		bool WasKeyPressed(const InputKey& key) const;
		bool WasKeyReleased(const InputKey& key) const;

		void ClearAllInput();
		
	private:
		void ProcessEventInternal(const sf::Event& evt, EntityManager& entityManager);

		void ProcessKeyPressed(const sf::Event::KeyPressed& evt, EntityManager& em);
		void ProcessKeyReleased(const sf::Event::KeyReleased& evt, EntityManager& em);
		void ProcessMouseButtonPressed(const sf::Event::MouseButtonPressed& evt, EntityManager& em);
		void ProcessMouseButtonReleased(const sf::Event::MouseButtonReleased& evt, EntityManager& em);
		void ProcessMouseMoved(const sf::Event::MouseMoved& evt, EntityManager& em);
		void ProcessMouseWheelScrolled(const sf::Event::MouseWheelScrolled& evt, EntityManager& em);
		void ProcessJoystickButtonPressed(const sf::Event::JoystickButtonPressed& evt, EntityManager& em);
		void ProcessJoystickButtonReleased(const sf::Event::JoystickButtonReleased& evt, EntityManager& em);
		void ProcessJoystickMoved(const sf::Event::JoystickMoved& evt, EntityManager& em);

		void ProcessOngoingInputs(TimeStep ts, EntityManager& em);
		void ApplySmoothing(TimeStep ts);

		void TriggerAction(const InputAction& action, ETriggerEvent trigger, const InputValue& value, EntityManager& em);

		void ProcessMapping(const InputMapping& mapping, const InputKey& key, bool pressed, EntityManager& em);
		void ProcessChordMapping(const ChordMapping& mapping, EntityManager& em);
		void ProcessComboMapping(const ComboMapping& mapping, const InputKey& key, EntityManager& em);
		void ProcessAxisMapping(const AxisMapping& mapping, EntityManager& em);
		void ProcessAxis2DMapping(const Axis2DMapping& mapping, EntityManager& em);

		// Helpers
		bool AreChordKeysPressed(const std::vector<InputKey>& keys) const;
		InputValue ApplyModifierConfig(const InputValue& value, const ModifierConfig& config);
		std::vector<Entity*> GetSortedInputEntities(EntityManager& em);

	private:
		InputManager() = default;

		float m_CurrentTime;

		InputContextStack m_ContextStack;
		std::unordered_map<InputKey, KeyState> m_KeyStates;
	};
}