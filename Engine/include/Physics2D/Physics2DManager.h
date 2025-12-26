#pragma once

#include "EngineAPI.h"
#include "Core/TimeStep.h"

#include <box2d/box2d.h>

namespace Luden
{
	class Scene;

	class ENGINE_API Physics2DManager
	{
	public:
		Physics2DManager() = default;

		Physics2DManager(const Physics2DManager&) = delete;
		Physics2DManager& operator=(const Physics2DManager&) = delete;

		void Init(Scene* scene, uint32_t viewportWidth, uint32_t viewportHeight);
		void Update(TimeStep ts);
		void Shutdown();

		b2WorldId GetPhysicsWorldId() { return m_PhysicsWorldId; }
		void SetPhysicsWorldId(b2WorldId physicsWorldId) { m_PhysicsWorldId = physicsWorldId; }

		float GetPhysicsScale() { return m_PhysicsScale; }
		void SetPhysicsScale(float physicsScale) { m_PhysicsScale = physicsScale; }

		b2Vec2 GetGravity() { return m_Gravity; }
		void SetGravity(b2Vec2 gravity) { m_Gravity = gravity; }

		int GetSubStepCount() { return m_SubStepCount; }
		void SetSubStepCount(int subStepCount) { m_SubStepCount = subStepCount; }

	private:
		Scene* m_Scene;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		//Physics2D
		b2WorldId m_PhysicsWorldId = b2_nullWorldId;
		float m_PhysicsScale = 100.0f; // 1 meter = 100 pixel
		b2Vec2 m_Gravity = { 0.0f, -10.0f };
		int m_SubStepCount = 4;
	};
}