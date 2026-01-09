#include "BallController.h"

#include "GameManager.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/AudioAPI.h"
#include "ScriptAPI/DebugAPI.h"
#include "ScriptAPI/MathAPI.h"
#include <iostream>

namespace Luden
{
    void BallController::OnCreate()
    {
		m_PaddleHitSound = GetResource<Sound>("paddle_hit");
		m_BrickHitSound = GetResource<Sound>("brick_hit");
		m_WallHitSound = GetResource<Sound>("wall_hit");

		m_Paddle = GameplayAPI::FindEntityWithTag("Player");

		Reset();
	}

    void BallController::OnUpdate(TimeStep ts)
    {
		if (!m_IsLaunched && m_Paddle.IsValid())
		{
			Vec3 paddlePos = GameplayAPI::GetPosition(m_Paddle);
			paddlePos.y -= 300.0f;  
			GameplayAPI::SetPosition(GetEntity(), paddlePos);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space))
			{
				Launch();
			}

			return;
		}

		Vec3 pos = GameplayAPI::GetPosition(GetEntity());
		if (pos.y > 600.0f)
		{
			Entity gm = GameplayAPI::FindEntityWithTag("GameManager");
			if (gm.IsValid() && gm.Has<NativeScriptComponent>())
			{
				auto& nsc = gm.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					auto* manager = dynamic_cast<GameManager*>(nsc.Instance);
					if (manager)
					{
						manager->LoseLife();
					}
				}
			}

			Reset();
		}

		Vec2 vel = Physics2DAPI::GetLinearVelocity(GetEntity());
		float speed = MathAPI::Length(vel);

		if (speed > m_Speed * 1.5f)
		{
			vel = MathAPI::Normalize(vel) * m_Speed;
			Physics2DAPI::SetLinearVelocity(GetEntity(), vel);
		}
		else if (speed < m_Speed * 0.8f && m_IsLaunched)
		{
			vel = MathAPI::Normalize(vel) * m_Speed;
			Physics2DAPI::SetLinearVelocity(GetEntity(), vel);
		}
	}

    void BallController::OnDestroy()
    {
        // TODO: Cleanup
    }

    void BallController::OnCollisionBegin(const CollisionContact& contact)
	{
		Entity other = contact.otherEntity;

		Vec2 velocity = Physics2DAPI::GetLinearVelocity(GetEntity());
		Vec2 normal = contact.normal;

		if (other.Tag() == "Player")
		{
			Vec3 ballPos = GameplayAPI::GetPosition(GetEntity());
			Vec3 paddlePos = GameplayAPI::GetPosition(other);

			float offset = ballPos.x - paddlePos.x;
			float paddleHalfWidth = 100.0f;
			float normalizedOffset = MathAPI::Clamp(offset / paddleHalfWidth, -1.0f, 1.0f);

			float angle = normalizedOffset * 60.0f; 
			float angleRad = angle * 3.14159265f / 180.0f;

			Vec2 newVelocity(
				std::sin(angleRad) * m_Speed,
				-std::abs(std::cos(angleRad)) * m_Speed  
			);

			Physics2DAPI::SetLinearVelocity(GetEntity(), newVelocity);

			if (m_PaddleHitSound)
			{
				std::cout << "PaddleHit Sound started" << std::endl;
				SoundAPI::PlaySound(m_PaddleHitSound, 0.5f);
			}

			std::cout << "[Ball] Hit paddle | Offset: " << normalizedOffset
				<< " | Angle: " << angle
				<< " | Normal: (" << normal.x << ", " << normal.y << ")" << std::endl;
		}
		else if (other.Tag() == "Brick" || other.Tag() == "Wall")
		{
			float dotProduct = velocity.x * normal.x + velocity.y * normal.y;
			Vec2 reflection(
				velocity.x - 2.0f * dotProduct * normal.x,
				velocity.y - 2.0f * dotProduct * normal.y
			);

			reflection = MathAPI::Normalize(reflection) * m_Speed;
			Physics2DAPI::SetLinearVelocity(GetEntity(), reflection);

			if (other.Tag() == "Brick" && m_BrickHitSound)
				SoundAPI::PlaySound(m_BrickHitSound, 0.6f);
			else if (other.Tag() == "Wall" && m_WallHitSound)
				SoundAPI::PlaySound(m_WallHitSound, 0.3f);

			std::cout << "[Ball] Reflected from " << other.Tag()
				<< " | Normal: (" << normal.x << ", " << normal.y << ")" << std::endl;
		}
	}

    void BallController::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void BallController::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

	void BallController::Launch()
	{
		m_IsLaunched = true;

		float angle = MathAPI::RandomFloat(-45.0f, 45.0f);
		float angleRad = angle * 3.14159265f / 180.0f;

		Vec2 velocity(
			std::sin(angleRad) * m_Speed,
			-std::abs(std::cos(angleRad)) * m_Speed  
		);

		Physics2DAPI::SetLinearVelocity(GetEntity(), velocity);
	}

	void BallController::Reset()
	{
		m_IsLaunched = false;
		Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0, 0));
	}
}
