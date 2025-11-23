#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>

namespace Luden
{
	enum class ETriggerEvent : uint8_t
	{
		Started,
		Ongoing,
		Triggered,
		Completed,
		Cancelled
	};

	enum class ETriggerType : uint8_t
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

	enum class EInputValueType : uint8_t
	{
		Boolean,
		Axis1D,		// Single float
		Axis2D,		// 2D vector
		Axis3D		// 3D vector
	};
	struct InputValue
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

	};

	using InputCallback = std::function<void(ETriggerEvent, const InputValue&)>;
}