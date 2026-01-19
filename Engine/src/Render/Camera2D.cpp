#include "Render/Camera2D.h"
#include "ECS/Entity.h"

#include <numbers>
#include <cmath>

namespace Luden
{
	Camera2D::Camera2D(glm::vec2 position, glm::vec2 viewportSize)
		: m_Position(position)
	{
		m_View.setCenter(sf::Vector2f(position.x, position.y));
		m_View.setSize(sf::Vector2f(viewportSize.x, viewportSize.y));
	}

	const char* Camera2D::CameraTypeToString(Type type)
	{
		switch (type)
		{
		case Type::FollowXY: return "FollowXY";
		case Type::Box:      return "Box";
		case Type::None:     return "None";
		}
		return "None";
	}

	Camera2D::Type Camera2D::CameraTypeFromString(const std::string& str)
	{
		if (str == "FollowXY") return Type::FollowXY;
		if (str == "Box")      return Type::Box;
		return Type::None;

	}

	float Camera2D::EvaluateOscillator(Oscillator& osc, float time)
	{
		if (osc.amplitude == 0.0f || osc.frequency == 0.0f)
			return 0.0f;

		return std::sin(2 * std::numbers::pi * osc.frequency * time) * osc.amplitude;
	}

	float Camera2D::CalculateBlendWeight(float elapsed, float duration, float blendIn, float blendOut)
	{
		float remaining = duration - elapsed;

		if (elapsed < blendIn && blendIn > 0.0f)
		{
			return elapsed / blendIn;
		}
		else if (remaining < blendOut && blendOut > 0.0f)
		{
			return remaining / blendOut;
		}

		return 1.0f;
	}

	void Camera2D::Shake(const CameraShakeParams& params, float scale)
	{
		m_ActiveShake.params = params;
		m_ActiveShake.scale = scale;
		m_ActiveShake.active = true;
		m_ActiveShake.elapsed = 0.0f;
	}

	void Camera2D::StopShake()
	{
		m_ActiveShake.active = false;
		m_ShakeOffset = { 0.0f, 0.0f };
		m_ShakeRotation = 0.0f;
		m_ShakeZoom = 0.0f;
	}

	void Camera2D::Update(TimeStep ts)
	{
		switch (m_Type)
		{	
		case Luden::Camera2D::Type::None:
		{
			m_View.setCenter(sf::Vector2f(m_Position.x, m_Position.y));
			break;
		}
		case Luden::Camera2D::Type::FollowXY:
		{
			if (m_AttachedEntity != nullptr && m_AttachedEntity->IsValid() && 
				m_AttachedEntity->Has<Camera2DComponent>() && m_AttachedEntity->Has<TransformComponent>())
			{
				glm::vec3 position = m_AttachedEntity->Get<TransformComponent>().Translation;
				m_Position = glm::vec2(position.x, position.y);
				m_View.setCenter(sf::Vector2f(m_Position.x, m_Position.y));
			}
			break;
		}
		case Luden::Camera2D::Type::Box:
			break;
		default:
			break;
		}


		// CameraShake logic
		m_ShakeOffset = { 0.0f, 0.0f };
		m_ShakeRotation = 0.0f;
		m_ShakeZoom = 0.0f;

		if (m_ActiveShake.active)
		{
			m_ActiveShake.elapsed += ts;

			if (m_ActiveShake.elapsed >= m_ActiveShake.params.duration)
			{
				StopShake();
			}
			else
			{
				auto& params = m_ActiveShake.params;
				float time = m_ActiveShake.elapsed;
				float scale = m_ActiveShake.scale;

				float blend = CalculateBlendWeight(time, params.duration, params.blendInTime, params.blendOutTime);

				float finalScale = scale * blend;

				m_ShakeOffset.x = EvaluateOscillator(params.locOscillationX, time) * finalScale;
				m_ShakeOffset.y = EvaluateOscillator(params.locOscillationY, time) * finalScale;

				m_ShakeRotation = EvaluateOscillator(params.rotOscillation, time) * finalScale;

				m_ShakeZoom = EvaluateOscillator(params.zoomOscillation, time) * finalScale;
			}
		}

		float finalZoom = std::clamp(m_Zoom + m_ShakeZoom, m_MinZoom, m_MaxZoom);

		m_View.setCenter(sf::Vector2f(m_Position.x + m_ShakeOffset.x,m_Position.y + m_ShakeOffset.y));
		m_View.setSize(sf::Vector2f(m_ViewportSize.x * finalZoom, m_ViewportSize.y * finalZoom));
		m_View.setRotation(sf::degrees(m_Rotation + m_ShakeRotation));
	}

	void Camera2D::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (const auto* mousePressed = evt->getIf<sf::Event::MouseButtonPressed>())
		{
			if (mousePressed->button == sf::Mouse::Button::Middle && m_DragEnabled)
			{
				m_IsDragging = true;
				m_LastMousePos = mousePressed->position;
				m_DragStartPos = m_Position;
			}
		}
		else if (const auto* mouseReleased = evt->getIf<sf::Event::MouseButtonReleased>())
		{
			if (mouseReleased->button == sf::Mouse::Button::Middle)
			{
				m_IsDragging = false;
			}
		}
		else if (const auto* mouseMoved = evt->getIf<sf::Event::MouseMoved>())
		{
			if (m_IsDragging && m_DragEnabled)
			{
				sf::Vector2i delta = mouseMoved->position - m_LastMousePos;

				float dragSpeed = m_Zoom;

				m_Position.x -= delta.x * dragSpeed;
				m_Position.y -= delta.y * dragSpeed;

				m_LastMousePos = mouseMoved->position;
			}
		}
		else if (const auto* mouseWheel = evt->getIf<sf::Event::MouseWheelScrolled>())
		{
			if (m_ZoomEnabled)
			{
				float delta = mouseWheel->delta;
				float zoomFactor = 1.0f - (delta * m_ZoomSpeed);

				sf::Vector2f mouseWorld = m_View.getCenter() +
					sf::Vector2f(
						(mouseWheel->position.x - m_ViewportSize.x * 0.5f) * m_Zoom,
						(mouseWheel->position.y - m_ViewportSize.y * 0.5f) * m_Zoom
					);

				m_Zoom *= zoomFactor;
				m_Zoom = std::clamp(m_Zoom, m_MinZoom, m_MaxZoom);

				sf::Vector2f offset = mouseWorld - m_View.getCenter();
				m_Position.x += offset.x * (1.0f - zoomFactor);
				m_Position.y += offset.y * (1.0f - zoomFactor);
			}
		}
	}
}