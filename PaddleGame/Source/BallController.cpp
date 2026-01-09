#include "BallController.h"

#include "GameManager.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/AudioAPI.h"
#include "ScriptAPI/DebugAPI.h"
#include "ScriptAPI/MathAPI.h"

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

		if (other.Tag() == "Player")
		{
			Vec3 ballPos = GameplayAPI::GetPosition(GetEntity());
			Vec3 paddlePos = GameplayAPI::GetPosition(other);

			float offset = ballPos.x - paddlePos.x;
			float maxOffset = 400.0f;
			float normalizedOffset = MathAPI::Clamp(offset / maxOffset, -1.0f, 1.0f);

			Vec2 vel = Physics2DAPI::GetLinearVelocity(GetEntity());
			vel.x += normalizedOffset * 5.0f;  
			vel.y = -std::abs(vel.y);  

			Physics2DAPI::SetLinearVelocity(GetEntity(), MathAPI::Normalize(vel) * m_Speed);

			if (m_PaddleHitSound)
				SoundAPI::PlaySound(m_PaddleHitSound, 0.5f);
		}
		else if (other.Tag() == "Brick")
		{
			if (m_BrickHitSound)
				SoundAPI::PlaySound(m_BrickHitSound, 0.6f);

		}
		else if (other.Tag() == "Wall")
		{
			if (m_WallHitSound)
				SoundAPI::PlaySound(m_WallHitSound, 0.3f);
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
			-std::cos(angleRad) * m_Speed
		);

		Physics2DAPI::SetLinearVelocity(GetEntity(), velocity);
	}

	void BallController::Reset()
	{
		m_IsLaunched = false;
		Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0, 0));
	}
}
