#pragma once

#include "EngineAPI.h"

#include <functional>
#include <variant>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>


namespace Luden
{
	enum class ENGINE_API ETriggerEvent : uint8_t
	{
		Started,
		Ongoing,
		Triggered,
		Completed,
		Cancelled
	};

	enum class ENGINE_API ETriggerType : uint8_t
	{
		Down,		// Basically every pressed
		Pressed,	// First frame pressed
		Released,	// When released
		Hold,		// After holding for a while (threshold)
		Tap,		// Quick press and release
		Pulse,		// Periodic trigger while holding
		Chord,		// Multiple keys pressed at the same time
		Combo		// Combo!
	};

	enum class ENGINE_API EInputValueType : uint8_t
	{
		Boolean,
		Axis1D,		// Single float
		Axis2D,		// 2D vector
		Axis3D		// 3D vector
	};
	struct ENGINE_API InputValue
	{
		EInputValueType type = EInputValueType::Boolean;

		union 
		{
			bool boolValue;
			float axis1D;
			glm::vec2 axis2D;
			glm::vec3 axis3D;
		};

		// Constructors
		InputValue() : boolValue(false) { }

		explicit InputValue(float value)
			: type(EInputValueType::Axis1D), axis1D(value)
		{ }

		explicit InputValue(glm::vec2 value)
			: type(EInputValueType::Axis2D), axis2D(value)
		{ }

		explicit InputValue(glm::vec3 value)
			: type(EInputValueType::Axis3D), axis3D(value)
		{ }

		//Getters
		bool GetBool() const { return boolValue; }
		float GetAxis1D() const { return axis1D; }
		glm::vec2 GetAxis2D() const { return axis2D; }
		glm::vec2 GetAxis3D() const { return axis3D; }

		float GetMagnitude() const;

		InputValue ApplyDeadZone(float deadZone) const;
		InputValue Normalize() const;
		InputValue Invert() const;
	};

	struct ENGINE_API ModifierConfig
	{
		float scale = 1.0f;
		float deadZone = 0.0f;
		float sensitivity = 1.0f;
		bool invert = false;
		bool normalize = false;
		float smoothing = 0.0f;

		ModifierConfig() = default;
	};

	struct ENGINE_API TriggerConfig
	{
		ETriggerType type = ETriggerType::Down;
		float holdTime = 0.5f;
		float tapTime = 0.3f;
		float pulseInterval = 0.1f;
		float activationThreshold = 0.5f;

		TriggerConfig() = default;
		explicit TriggerConfig(ETriggerType t) : type(t) {}
	};

	using InputCallback = std::function<void(ETriggerEvent, const InputValue&)>;
}