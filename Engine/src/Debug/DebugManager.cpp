#include "Debug/DebugManager.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "box2d/box2d.h"

namespace Luden
{
	void DebugManager::DrawLine(const glm::vec3& start, const glm::vec3& end, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;

		cmd.type = DebugDrawCommand::Type::Line;
		cmd.p1 = start;
		cmd.p2 = end;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}

	void DebugManager::DrawCircle(const glm::vec3& center, float radius, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;
		cmd.type = DebugDrawCommand::Type::Circle;
		cmd.p1 = center;
		cmd.radius = radius;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}

	void DebugManager::DrawBox(const glm::vec3& center, const glm::vec3& extent, const sf::Color& color, float duration)
	{
		DebugDrawCommand cmd;
		cmd.type = DebugDrawCommand::Type::Box;
		cmd.p1 = center;
		cmd.p2 = extent;
		cmd.color = color;
		cmd.duration = duration;
		cmd.timeRemaining = duration;

		m_Commands.push_back(cmd);
	}

	void DebugManager::Update(float deltaTime)
	{
		for (auto it = m_Commands.begin(); it != m_Commands.end();)
		{
			if (it->duration == 0.0f && it->rendered == true)
			{
				it = m_Commands.erase(it);
				continue;

			}
			else if (it->duration > 0.0f)
			{
				it->timeRemaining -= deltaTime;
				if (it->timeRemaining <= 0.0f)
				{
					it = m_Commands.erase(it);
					continue;
				}
			}
			it++;
		}
	}

	void DebugManager::Render(std::shared_ptr<sf::RenderTexture> target)
	{
		for (auto& cmd : m_Commands)
		{
			sf::Color sfColor(
				(cmd.color.r),
				(cmd.color.g),
				(cmd.color.b),
				(cmd.color.a)
			);

			switch (cmd.type)
			{
			case DebugDrawCommand::Type::Line:
			{
				sf::VertexArray lines(sf::PrimitiveType::Lines, 2);
				lines[0].position = sf::Vector2f(cmd.p1.x, cmd.p1.y);
				lines[1].position = sf::Vector2f(cmd.p2.x, cmd.p2.y);
				lines[0].color = sfColor;
				lines[1].color = sfColor;
				target->draw(lines);
				break;
			}
			case DebugDrawCommand::Type::Circle:
			{
				sf::CircleShape circle(cmd.radius);
				circle.setPosition(sf::Vector2f(cmd.p1.x - cmd.radius, cmd.p1.y - cmd.radius));
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineColor(sfColor);
				circle.setOutlineThickness(4.0f);
				target->draw(circle);
				break;
			}

			case DebugDrawCommand::Type::Box:
			{
				sf::RectangleShape box(sf::Vector2f(cmd.p2.x * 2, cmd.p2.y * 2));
				box.setPosition(sf::Vector2f(cmd.p1.x - cmd.p2.x, cmd.p1.y - cmd.p2.y));
				box.setFillColor(sf::Color::Transparent);
				box.setOutlineColor(sfColor);
				box.setOutlineThickness(4.0f);
				target->draw(box);
				break;
			}
			default:
				break;
			}

			cmd.rendered = true;
		}
	}
	void DebugManager::Clear()
	{
		m_Commands.clear();
	}

	void DebugManager::SetDebugDraw(b2WorldId worldId, float physicsScale, uint32_t viewportHeight, uint32_t viewportWidth)
	{
		m_B2DebugContext.worldId = worldId;
		m_B2DebugContext.physicsScale = physicsScale;
		m_B2DebugContext.viewportWidth = viewportWidth;
		m_B2DebugContext.viewportHeight = viewportHeight;
	}

	void DebugManager::DebugDrawPhysics2D(b2WorldId worldId)
	{
		if (b2World_IsValid(worldId))
		{
			b2DebugDraw draw;
			
			draw = b2DefaultDebugDraw();

			draw.forceScale = m_DebugConfig.forceScale; 
			draw.jointScale = m_DebugConfig.jointScale;
			draw.drawShapes = m_DebugConfig.drawShapes;
			draw.drawJoints = m_DebugConfig.drawJoints;
			draw.drawJointExtras = m_DebugConfig.drawJointExtras;
			draw.drawBounds = m_DebugConfig.drawBounds;
			draw.drawMass = m_DebugConfig.drawMass;
			draw.drawBodyNames = m_DebugConfig.drawBodyNames;
			draw.drawContactPoints = m_DebugConfig.drawContactPoints;
			draw.drawGraphColors = m_DebugConfig.drawGraphColors;
			draw.drawContactFeatures = m_DebugConfig.drawContactFeatures;
			draw.drawContactNormals = m_DebugConfig.drawContactNormals;
			draw.drawContactForces = m_DebugConfig.drawContactForces;
			draw.drawFrictionForces = m_DebugConfig.drawFrictionForces;
			draw.drawIslands = m_DebugConfig.drawIslands;

			draw.context = &m_B2DebugContext;

			draw.DrawPolygonFcn = &Box2DDrawPolygon;
			draw.DrawSolidPolygonFcn = &Box2DDrawSolidPolygon;
			draw.DrawCircleFcn= &Box2DDrawCircle;
			draw.DrawSolidCircleFcn = &Box2DDrawSolidCircle;
			draw.DrawSolidCapsuleFcn = &Box2DDrawSolidCapsule;
			draw.DrawLineFcn = &Box2DDrawLine;
			draw.DrawTransformFcn = &Box2DDrawTransform;
			draw.DrawPointFcn = &Box2DDrawPoint;
			draw.DrawStringFcn = &Box2DDrawString;
		
			b2World_Draw(worldId, &draw);
		}
	}

	void DebugManager::Box2DDrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		for (int i = 0; i < vertexCount; i++)
		{
			int next = (i + 1) % vertexCount;
			glm::vec3 p1 = TransformB2Point(vertices[i], ctx);
			glm::vec3 p2 = TransformB2Point(vertices[next], ctx);

			Instance().DrawLine(p1, p2, sfColor, 0.0f);
		}
	}

	void DebugManager::Box2DDrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);
		sf::Color fillColor(sfColor.r, sfColor.g, sfColor.b, 100); 

		for (int i = 0; i < vertexCount; i++)
		{
			int next = (i + 1) % vertexCount;

			b2Vec2 v1 = b2TransformPoint(transform, vertices[i]);
			b2Vec2 v2 = b2TransformPoint(transform, vertices[next]);

			glm::vec3 p1 = TransformB2Point(v1, ctx);
			glm::vec3 p2 = TransformB2Point(v2, ctx);

			Instance().DrawLine(p1, p2, sfColor, 0.0f);
		}

		// Center point
		if (vertexCount > 0)
		{
			b2Vec2 center = b2TransformPoint(transform, b2Vec2{ 0, 0 });
			glm::vec3 centerPos = TransformB2Point(center, ctx);
			Instance().DrawCircle(centerPos, 2.0f, sf::Color::Red, 0.0f);
		}
	}

	void DebugManager::Box2DDrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		glm::vec3 centerPos = TransformB2Point(center, ctx);
		Instance().DrawCircle(centerPos, radius * ctx->physicsScale, sfColor, 0.0f);
	}

	void DebugManager::Box2DDrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		b2Vec2 center = b2TransformPoint(transform, b2Vec2{ 0, 0 });
		glm::vec3 centerPos = TransformB2Point(center, ctx);

		Instance().DrawCircle(centerPos, radius * ctx->physicsScale, sfColor, 0.0f);

		b2Vec2 axis = b2RotateVector(transform.q, b2Vec2{ radius, 0 });
		b2Vec2 endPoint = b2Add(center, axis);
		glm::vec3 axisEnd = TransformB2Point(endPoint, ctx);

		Instance().DrawLine(centerPos, axisEnd, sf::Color::Red, 0.0f);
	}

	void DebugManager::Box2DDrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		glm::vec3 point1 = TransformB2Point(p1, ctx);
		glm::vec3 point2 = TransformB2Point(p2, ctx);

		// Draw line between centers
		Instance().DrawLine({ point1.x - radius, point1.y, point1.z}, { point2.x - radius, point2.y, point2.z }, sfColor, 0.0f);
		Instance().DrawLine({ point1.x + radius, point1.y, point1.z }, { point2.x + radius, point2.y, point2.z }, sfColor, 0.0f);

		// Draw circles at ends
		Instance().DrawCircle(point1, radius * ctx->physicsScale, sfColor, 0.0f);
		Instance().DrawCircle(point2, radius * ctx->physicsScale, sfColor, 0.0f);
	}

	void DebugManager::Box2DDrawLine(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		glm::vec3 point1 = TransformB2Point(p1, ctx);
		glm::vec3 point2 = TransformB2Point(p2, ctx);

		Instance().DrawLine(point1, point2, sfColor, 0.0f);
	}

	void DebugManager::Box2DDrawTransform(b2Transform transform, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);

		const float axisScale = 0.4f * ctx->physicsScale;
		b2Vec2 origin = transform.p;

		b2Vec2 xAxis = b2RotateVector(transform.q, b2Vec2{ axisScale, 0 });
		b2Vec2 xEnd = b2Add(origin, xAxis);
		glm::vec3 p1 = TransformB2Point(origin, ctx);
		glm::vec3 p2 = TransformB2Point(xEnd, ctx);
		Instance().DrawLine(p1, p2, sf::Color::Red, 0.0f);

		b2Vec2 yAxis = b2RotateVector(transform.q, b2Vec2{ 0, axisScale });
		b2Vec2 yEnd = b2Add(origin, yAxis);
		glm::vec3 p3 = TransformB2Point(yEnd, ctx);
		Instance().DrawLine(p1, p3, sf::Color::Green, 0.0f);
	}

	void DebugManager::Box2DDrawPoint(b2Vec2 p, float size, b2HexColor color, void* context)
	{
		auto* ctx = static_cast<Box2DDebugContext*>(context);
		sf::Color sfColor = ConvertColor(color);

		glm::vec3 point = TransformB2Point(p, ctx);
		Instance().DrawCircle(point, size * ctx->physicsScale, sfColor, 0.0f);
	}

	void DebugManager::Box2DDrawString(b2Vec2 p, const char* s, b2HexColor color, void* context)
	{
		// TODO: gonna use sf::text
	}

	sf::Color DebugManager::ConvertColor(b2HexColor color)
	{
		return sf::Color(
			static_cast<uint8_t>((color >> 16) & 0xFF),
			static_cast<uint8_t>((color >> 8) & 0xFF),
			static_cast<uint8_t>(color & 0xFF),
			static_cast<uint8_t>((color >> 24) & 0xFF)
		);
	}

	glm::vec3 DebugManager::TransformB2Point(const b2Vec2& point, const Box2DDebugContext* ctx)
	{
		return glm::vec3(
			point.x * ctx->physicsScale,
			ctx->viewportHeight - (point.y * ctx->physicsScale),
			0.0f
		);
	}
}