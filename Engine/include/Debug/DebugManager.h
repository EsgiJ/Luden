	#pragma once

	#include "EngineAPI.h"

	#include <memory>

	#include <glm/vec3.hpp>
	#include <SFML/Graphics/Color.hpp>
	#include "SFML/Graphics/RenderTexture.hpp"
	#include <box2d/box2d.h>

	namespace Luden
	{
		struct ENGINE_API Box2DDebugContext
		{
			float physicsScale = 0.0f;
			uint32_t viewportHeight = 0;
			uint32_t viewportWidth = 0;
			b2WorldId worldId = b2_nullWorldId;
		};

		struct ENGINE_API Box2DDebugConfig
		{
			float forceScale = 1.0f;
			float jointScale = 1.0f;
			bool drawShapes = false;
			bool drawJoints = false;
			bool drawJointExtras = false;
			bool drawBounds = false;
			bool drawMass = false;
			bool drawBodyNames = false;
			bool drawContactPoints = false;
			bool drawGraphColors = false;
			bool drawContactFeatures = false;
			bool drawContactNormals = false;
			bool drawContactForces = false;
			bool drawFrictionForces = false;
			bool drawIslands = false;
			bool enabled = false; 
		};

		struct ENGINE_API DebugDrawCommand
		{
			enum class Type
			{
				Line,
				Box,
				Circle
			};

			Type type;
			glm::vec3 p1;
			glm::vec3 p2;

			float radius;
			sf::Color color;
			float duration;
			float timeRemaining;
			bool rendered = false;
		};

		class ENGINE_API DebugManager
		{
		public:
			static DebugManager& Instance()
			{
				static DebugManager s_Instance;
				return s_Instance;
			}

			DebugManager(const DebugManager&) = delete;
			DebugManager& operator =(const DebugManager&) = delete;

			void DrawLine(const glm::vec3& start, const glm::vec3& end, const sf::Color& color, float duration);

			void DrawCircle(const glm::vec3& center, float radius, const sf::Color& color, float duration);

			void DrawBox(const glm::vec3& center, const glm::vec3& extent, const sf::Color& color, float duration);

			void Update(float deltaTime);
			void Render(std::shared_ptr<sf::RenderTexture> target);
			void Clear();

			Box2DDebugConfig& GetDebugConfig() { return m_DebugConfig; }

		private:
			void SetDebugDraw(b2WorldId worldId, float physicsScale, uint32_t viewportHeight, uint32_t viewportWidth);
			void DebugDrawPhysics2D(b2WorldId worldId);

			// Box2D debug funcs
			static void Box2DDrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
			static void Box2DDrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context);
			static void Box2DDrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context);
			static void Box2DDrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context);
			static void Box2DDrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context);
			static void Box2DDrawLine(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context);
			static void Box2DDrawTransform(b2Transform transform, void* context);
			static void Box2DDrawPoint(b2Vec2 p, float size, b2HexColor color, void* context);
			static void Box2DDrawString(b2Vec2 p, const char* s, b2HexColor color, void* context);

			static sf::Color ConvertColor(b2HexColor color);
			static glm::vec3 TransformB2Point(const b2Vec2& point, const Box2DDebugContext* ctx);

		private:
			DebugManager() = default;

			std::vector<DebugDrawCommand> m_Commands;

			Box2DDebugConfig m_DebugConfig;
			Box2DDebugContext m_B2DebugContext;

			friend class Scene;
			friend class Physics2DManager;
		};
	}