#include "Render/Camera2D.h"
#include "ECS/Entity.h"

namespace Luden
{
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

	Camera2D::Camera2D(glm::vec2 position, glm::vec2 viewportSize)
		: m_Position(position)
	{
		m_View.setCenter(sf::Vector2f(position.x, position.y));
		m_View.setSize(sf::Vector2f(viewportSize.x, viewportSize.y));
	}

	void Camera2D::Update()
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
		m_Zoom = std::clamp(m_Zoom, m_MinZoom, m_MaxZoom);

		m_View.setSize(sf::Vector2f(m_ViewportSize.x * m_Zoom, m_ViewportSize.y * m_Zoom));
		m_View.setRotation(sf::degrees(m_Rotation));
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