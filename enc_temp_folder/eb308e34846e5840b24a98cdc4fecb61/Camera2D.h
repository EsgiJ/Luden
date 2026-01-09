#pragma once

#include "EngineAPI.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <optional>

namespace Luden
{
	class Entity;

	class ENGINE_API Camera2D
	{
	public:
		enum class Type
		{
			None = -1,
			FollowXY,
			Box
		};

		Camera2D() = default;
		explicit Camera2D(glm::vec2 position, glm::vec2 viewportSize);

		Entity* GetAttachedEntity() { return m_AttachedEntity; }
		void SetAttachedEntity(Entity* entity) { m_AttachedEntity = entity; }

		Type GetCameraType() const { return m_Type; }
		void SetCameraType(Type type) { m_Type = type; }

		glm::vec2& GetPosition() { return m_Position; }
		void SetPosition(const glm::vec2& pos) { m_Position = pos; }

		float GetRotation() { return m_Rotation; }
		void SetRotation(float angleDeg) { m_Rotation = angleDeg; }

		float GetZoom() { return m_Zoom; }
		void SetZoom(float zoom) { m_Zoom = zoom; }

		void SetViewportSize(const glm::vec2& size) { m_ViewportSize = size; }
		glm::vec2& GetViewportSize() { return m_ViewportSize; }

		sf::View& GetView() { return m_View; }

		void Update();

		void OnEvent(const std::optional<sf::Event>& evt);

		void SetDragEnabled(bool enabled) { m_DragEnabled = enabled; }
		bool IsDragEnabled() const { return m_DragEnabled; }

		void SetZoomEnabled(bool enabled) { m_ZoomEnabled = enabled; }
		bool IsZoomEnabled() const { return m_ZoomEnabled; }

		void SetMinZoom(float minZoom) { m_MinZoom = minZoom; }
		void SetMaxZoom(float maxZoom) { m_MaxZoom = maxZoom; }

		static const char* CameraTypeToString(Type type);
		static Type CameraTypeFromString(const std::string& str);
	private:
		Entity* m_AttachedEntity = nullptr;
		Type m_Type = Type::None;

		sf::View m_View;
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
		glm::vec2 m_Position = { 0.0f, 0.0f };
		float m_Rotation = 0.0f;
		float m_Zoom = 1.0f;

		bool m_DragEnabled = true;
		bool m_IsDragging = false;
		sf::Vector2i m_LastMousePos;
		glm::vec2 m_DragStartPos;

		bool m_ZoomEnabled = true;
		float m_MinZoom = 0.1f;
		float m_MaxZoom = 10.0f;
		float m_ZoomSpeed = 0.1f;
	};
}