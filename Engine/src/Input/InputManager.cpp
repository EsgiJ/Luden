#include "Input/InputManager.h"
#include "ECS/EntityManager.h"
#include "Input/InputAction.h"
#include "Input/InputMapping.h"
#include "ECS/Components/Components.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <variant>

namespace Luden
{
	bool InputManager::AreChordKeysPressed(const std::vector<InputKey>& keys) const
	{
		for (const auto& key : keys)
		{
			if (!IsKeyDown(key))
				return false;
		}
		return true;
	}

	InputValue InputManager::ApplyModifierConfig(const InputValue& value, const ModifierConfig& config)
	{
		InputValue result = value;

		// Dead zone
		if (config.deadZone > 0.0f)
		{
			result = result.ApplyDeadZone(config.deadZone);
		}

		// Scale
		if (config.scale != 1.0f)
		{
			switch (result.type)
			{
			case EInputValueType::Axis1D:
				result.axis1D *= config.scale;
				break;
			case EInputValueType::Axis2D:
				result.axis2D *= config.scale;
				break;
			case EInputValueType::Axis3D:
				result.axis3D *= config.scale;
				break;
			default:
				break;
			}
		}

		// Sensitivity
		if (config.sensitivity != 1.0f)
		{
			switch (result.type)
			{
			case EInputValueType::Axis1D:
				result.axis1D *= config.sensitivity;
				break;
			case EInputValueType::Axis2D:
				result.axis2D *= config.sensitivity;
				break;
			case EInputValueType::Axis3D:
				result.axis3D *= config.sensitivity;
				break;
			default:
				break;
			}
		}

		// Invert
		if (config.invert)
		{
			result = result.Invert();
		}

		// Normalize
		if (config.normalize)
		{
			result = result.Normalize();
		}

		return result;
	}

	std::vector<Entity*> InputManager::GetSortedInputEntities(EntityManager& em)
	{
		std::vector<Entity*> entities;

		for (auto& entity : em.GetEntities())
		{
			if (entity.Has<InputComponent>())
			{
				auto& input = entity.Get<InputComponent>();
				if (input.enabled)
					entities.push_back(&entity);
			}
		}

		std::sort(entities.begin(), entities.end(),
			[](const Entity* a, const Entity* b)
			{
				auto& inputA = a->Get<InputComponent>();
				auto& inputB = b->Get<InputComponent>();
				return inputA.priority > inputB.priority;
			});

		return entities;
	}

	void InputManager::PushContext(std::shared_ptr<InputContext> context)
	{
		m_ContextStack.PushContext(context);
	}

	void InputManager::PopContext(const std::string& name)
	{
		m_ContextStack.PopContext(name);
	}

	void InputManager::PopContext(std::shared_ptr<InputContext> context)
	{
		m_ContextStack.PopContext(context);
	}

	std::shared_ptr<InputContext> InputManager::FindContext(const std::string& name) const
	{
		return m_ContextStack.FindContext(name);
	}
	bool InputManager::IsKeyDown(const InputKey& key) const
	{
		auto it = m_KeyStates.find(key);
		return it != m_KeyStates.end() && it->second.pressed;
	}
	bool InputManager::WasKeyPressed(const InputKey& key) const
	{
		auto it = m_KeyStates.find(key);
		return it != m_KeyStates.end() && it->second.pressed && !it->second.wasPressed;
	}
	bool InputManager::WasKeyReleased(const InputKey& key) const
	{
		auto it = m_KeyStates.find(key);
		return it != m_KeyStates.end() && !it->second.pressed && it->second.wasPressed;
	}

	void InputManager::ClearAllInput()
	{
		m_KeyStates.clear();
		m_ContextStack.ClearContexts();
	}

	void InputManager::ProcessEvent(const sf::Event& evt, EntityManager& entityManager)
	{
		ProcessEventInternal(evt, entityManager);
	}

	void InputManager::ProcessEventInternal(const sf::Event& evt, EntityManager& entityManager)
	{
		if (auto e = evt.getIf<sf::Event::KeyPressed>())
			ProcessKeyPressed(*e, entityManager);
		else if (auto e = evt.getIf<sf::Event::KeyReleased>())
			ProcessKeyReleased(*e, entityManager);
	}

	void InputManager::Update(TimeStep ts, EntityManager& entityManager)
	{
		m_CurrentTime += ts;

		ProcessOngoingInputs(ts, entityManager);
		ApplySmoothing(ts);

		for (auto& [key, state] : m_KeyStates)
		{
			state.wasPressed = state.pressed;
		}
	}

	void InputManager::ProcessKeyPressed(const sf::Event::KeyPressed& evt, EntityManager& em)
	{
		InputKey key = evt.code;

		auto& state = m_KeyStates[key];
		bool wasAlreadyPressed = state.pressed;
		state.pressed = true;
		state.currentValue = InputValue(true);

		if (!wasAlreadyPressed)
		{
			state.pressedTime = m_CurrentTime;
		}

		auto contexts = m_ContextStack.GetActiveContexts();

		for (auto context : contexts)
		{
			if (!context->IsEnabled())
				continue;

			// Normal mappings
			for (const auto& mapping : context->GetMappings())
			{
				if (auto* mappedKey = std::get_if<sf::Keyboard::Key>(&mapping.key))
				{
					if (*mappedKey == evt.code)
					{
						ProcessMapping(mapping, key, true, em);
					}
				}
			}

			// Chord mappings
			for (const auto& chord : context->GetChordMappings())
			{
				ProcessChordMapping(chord, em);
			}

			// Combo mappings
			for (const auto& combo : context->GetComboMappings())
			{
				ProcessComboMapping(combo, key, em);
			}

			if (context->IsBlocking())
				break;            
		}
	}

	void InputManager::ProcessKeyReleased(const sf::Event::KeyReleased& evt, EntityManager& em)
	{
		InputKey key = evt.code;

		auto& state = m_KeyStates[key];
		state.pressed = false;
		state.releasedTime = m_CurrentTime;
		state.currentValue = InputValue(false);

		float holdDuration = m_CurrentTime - state.pressedTime;

		auto contexts = m_ContextStack.GetActiveContexts();

		for (auto* context : contexts)
		{
			if (!context->IsEnabled())
				continue;

			for (const auto& mapping : context->GetMappings())
			{
				if (auto* mappedKey = std::get_if<sf::Keyboard::Key>(&mapping.key))
				{
					if (*mappedKey == evt.code)
					{
						if (mapping.triggerConfig.type == ETriggerType::Released)
						{
							TriggerAction(mapping.action, ETriggerEvent::Completed, InputValue(false), em);
						}
						else if (mapping.triggerConfig.type == ETriggerType::Tap)
						{
							if (holdDuration <= mapping.triggerConfig.tapTime)
							{
								TriggerAction(mapping.action, ETriggerEvent::Triggered, InputValue(true), em);
							}
						}
						else if (mapping.triggerConfig.type == ETriggerType::Hold)
						{
							if (holdDuration >= mapping.triggerConfig.holdTime)
							{
								TriggerAction(mapping.action, ETriggerEvent::Completed, InputValue(true), em);
							}
						}
					}
				}
			}

			if (context->IsBlocking())
				break;
		}
	}

	void InputManager::ProcessMouseButtonPressed(const sf::Event::MouseButtonPressed& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessMouseButtonReleased(const sf::Event::MouseButtonReleased& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessMouseMoved(const sf::Event::MouseMoved& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessMouseWheelScrolled(const sf::Event::MouseWheelScrolled& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessJoystickButtonPressed(const sf::Event::JoystickButtonPressed& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessJoystickButtonReleased(const sf::Event::JoystickButtonReleased& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessJoystickMoved(const sf::Event::JoystickMoved& evt, EntityManager& em)
	{
	}

	void InputManager::ProcessOngoingInputs(TimeStep ts, EntityManager& em)
	{
		auto contexts = m_ContextStack.GetActiveContexts();

		for (const auto& [key, state] : m_KeyStates)
		{
			if (!state.pressed)
				continue;

			float pressedDuration = m_CurrentTime - state.pressedTime;

			for (auto* context : contexts)
			{
				if (!context->IsEnabled())
					continue;

				for (const auto& mapping : context->GetMappings())
				{
					switch (mapping.triggerConfig.type)
					{
					case ETriggerType::Down:
						TriggerAction(mapping.action, ETriggerEvent::Ongoing, state.currentValue, em);
						break;

					case ETriggerType::Hold:
						if (pressedDuration >= mapping.triggerConfig.holdTime &&
							!state.wasPressed)
						{
							TriggerAction(mapping.action, ETriggerEvent::Triggered, InputValue(true), em);
						}
						break;

					case ETriggerType::Pulse:
					{
						auto& mutableState = m_KeyStates[key];
						if (m_CurrentTime - mutableState.lastPulseTime >=
							mapping.triggerConfig.pulseInterval)
						{
							TriggerAction(mapping.action, ETriggerEvent::Triggered, InputValue(true), em);
							mutableState.lastPulseTime = m_CurrentTime;
						}
						break;
					}

					default:
						break;
					}
				}
			}
		}

		for (auto* context : contexts)
		{
			if (!context->IsEnabled())
				continue;

			for (const auto& axisMapping : context->GetAxisMappings())
			{
				ProcessAxisMapping(axisMapping, em);
			}

			for (const auto& axis2DMapping : context->GetAxis2DMappings())
			{
				ProcessAxis2DMapping(axis2DMapping, em);
			}
		}
	}

	void InputManager::ApplySmoothing(TimeStep ts)
	{
	}

	void InputManager::TriggerAction(const InputAction& action, ETriggerEvent trigger, const InputValue& value, EntityManager& em)
	{
		auto entities = GetSortedInputEntities(em);

		for (auto entity : entities)
		{
			if (!entity->IsActive())
				continue;

			auto& inputComp = entity->Get<InputComponent>();

			if (!inputComp.enabled)
				continue;

			inputComp.TriggerAction(action, trigger, value);

			if (inputComp.consumeInput)
				break;
		}
	}

	void InputManager::ProcessMapping(const InputMapping& mapping, const InputKey& key, bool pressed, EntityManager& em)
	{
		auto& state = m_KeyStates[key];

		switch (mapping.triggerConfig.type)
		{
		case ETriggerType::Down:
		case ETriggerType::Pressed:
			if (pressed && !state.wasPressed)
			{
				TriggerAction(mapping.action, ETriggerEvent::Started, InputValue(true), em);
			}
			break;

		case ETriggerType::Released:
			// Handled in ProcessKeyReleased/ProcessMouseButtonReleased
			break;

		case ETriggerType::Hold:
		case ETriggerType::Pulse:
			// Handled in ProcessOngoingInputs
			break;

		default:
			break;
		}
	}
	void InputManager::ProcessChordMapping(const ChordMapping& mapping, EntityManager& em)
	{
		if (AreChordKeysPressed(mapping.keys))
		{
			TriggerAction(mapping.action, ETriggerEvent::Triggered, InputValue(true), em);
		}
	}
	void InputManager::ProcessComboMapping(const ComboMapping& mapping, const InputKey& key, EntityManager& em)
	{
		//TODO:
	}
	void InputManager::ProcessAxisMapping(const AxisMapping& mapping, EntityManager& em)
	{
		float value = 0.0f;

		if (IsKeyDown(mapping.positiveKey))
			value += 1.0f;
		if (IsKeyDown(mapping.negativeKey))
			value -= 1.0f;

		if (value != 0.0f)
		{
			InputValue inputValue(value);
			inputValue = ApplyModifierConfig(inputValue, mapping.modifierConfig);
			TriggerAction(mapping.action, ETriggerEvent::Ongoing, inputValue, em);
		}
	}

	void InputManager::ProcessAxis2DMapping(const Axis2DMapping& mapping, EntityManager& em)
	{
		glm::vec2 value(0.0f, 0.0f);

		if (IsKeyDown(mapping.rightKey))
			value.x += 1.0f;
		if (IsKeyDown(mapping.leftKey))
			value.x -= 1.0f;
		if (IsKeyDown(mapping.upKey))
			value.y -= 1.0f;
		if (IsKeyDown(mapping.downKey))
			value.y += 1.0f;

		InputValue inputValue(value);
		inputValue = ApplyModifierConfig(inputValue, mapping.modifierConfig);
		TriggerAction(mapping.action, ETriggerEvent::Ongoing, inputValue, em);
	}
}