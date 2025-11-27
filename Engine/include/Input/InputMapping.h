#pragma once

#include <variant>

#include "Input/InputTypes.h"
#include "Input/InputAction.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>

namespace Luden
{
	using InputKey = std::variant<
		sf::Keyboard::Key,
		sf::Mouse::Button,
		sf::Mouse::Wheel
	>;

	struct ENGINE_API InputMapping
	{
		InputAction action;
		InputKey key;
		ModifierConfig modifierConfig;
		TriggerConfig triggerConfig;

		InputMapping(InputAction act, InputKey k, ModifierConfig m, TriggerConfig t)
			: action(act), key(k), modifierConfig(m), triggerConfig(t)
		{
		}
	};

	struct ENGINE_API ChordMapping
	{
		InputAction action;
		std::vector<InputKey> keys;
		ModifierConfig modifierConfig;
		TriggerConfig triggerConfig;

		ChordMapping(InputAction act, std::vector<InputKey> ks, ModifierConfig m, TriggerConfig t)
			: action(act), keys(ks), modifierConfig(m), triggerConfig(t)
		{
		}
	};

	struct ENGINE_API ComboMapping
	{
		InputAction action;
		std::vector<InputKey> sequence;
		float maxTimeBetweenKeys = 0.5f;

		ComboMapping(InputAction act, std::vector<InputKey> seq, float maxTime)
			: action(act), sequence(seq), maxTimeBetweenKeys(maxTime)
		{
		}
	};

	struct ENGINE_API AxisMapping
	{
		InputAction action;
		InputKey positiveKey;
		InputKey negativeKey;
		ModifierConfig modifierConfig;

		AxisMapping(InputAction act, InputKey pos, InputKey neg, const ModifierConfig& config)
			: action(std::move(act)), positiveKey(pos), negativeKey(neg), modifierConfig(config) {
		}
	};

	struct ENGINE_API Axis2DMapping
	{
		InputAction action;
		InputKey upKey;
		InputKey downKey;
		InputKey leftKey;
		InputKey rightKey;
		ModifierConfig modifierConfig;

		Axis2DMapping(InputAction act, InputKey up, InputKey down, InputKey left, InputKey right,
			const ModifierConfig& config)
			: action(act), upKey(up), downKey(down), leftKey(left), rightKey(right)
			, modifierConfig(config) {
		}
	};
}