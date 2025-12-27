#include "Input/InputTypes.h"

namespace Luden
{
	float InputValue::GetMagnitude() const
	{
		switch (type)
		{
		case EInputValueType::Boolean:
			return boolValue ? 1.0f : -1.0f;
		case EInputValueType::Axis1D:
			return std::abs(axis1D);
		case EInputValueType::Axis2D:
			return static_cast<float>(axis2D.length());
		case EInputValueType::Axis3D:
			return static_cast<float>(axis3D.length());
		}

		return 0.0f;
	}

	InputValue InputValue::ApplyDeadZone(float deadZone) const
	{
		InputValue result = *this;
		switch (type)
		{
		case EInputValueType::Axis1D:
		{
			if (GetMagnitude() < deadZone)
				result.axis1D = 0.0f;
			break;
		}
		case EInputValueType::Axis2D:
		{
			float mag = GetMagnitude();
			if (mag < deadZone)
				result.axis2D = { 0.0f, 0.0f };
			else
			{
				float scale = (mag - deadZone) / (1.0f - deadZone);
				result.axis2D = axis2D * (scale / mag);
			}
			break;
		}
		case EInputValueType::Axis3D:
		{
			float mag = GetMagnitude();
			if (mag < deadZone)
			{
				result.axis3D = { 0.0f, 0.0f, 0.0f };
			}
			else
			{
				float scale = (mag - deadZone) / (1.0f - deadZone);
				result.axis3D = axis3D * (scale / mag);
			}
			break;
		}
		default:
			break;
		}

		return result;
	}
	InputValue InputValue::Normalize() const
	{
		InputValue result = *this;
		switch (type)
		{
		case EInputValueType::Axis2D:
		{
			float mag = GetMagnitude();
			if (mag > 0.0f)
				result.axis2D = axis2D / mag;
			break;
		}
		case EInputValueType::Axis3D:
		{
			float mag = GetMagnitude();
			if (mag > 0.0f)
				result.axis3D = axis3D / mag;
			break;
		}
		default:
			break;
		}

		return result;
	}

	InputValue InputValue::Invert() const
	{
		InputValue result = *this;

		switch (type)
		{
		case EInputValueType::Axis1D:
			result.axis1D = -axis1D;
			break;

		case EInputValueType::Axis2D:
			result.axis2D = -axis2D;
			break;

		case EInputValueType::Axis3D:
			result.axis3D = -axis3D;
			break;

		default:
			break;
		}

		return result;
	}
}