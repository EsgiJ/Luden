#include "BallController.h"
#include "GameManager.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/AudioAPI.h"
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
			paddlePos.y -= 200.0f;
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
			return;
		}
		Vec2 vel = Physics2DAPI::GetLinearVelocity(GetEntity());

		float speed = MathAPI::Length(vel);
		if (speed < 0.1f) return; 

		bool velocityChanged = false;


		if (std::abs(vel.y) < 2.0f) 
		{
			float nudge = (vel.y >= 0.0f) ? 2.0f : -2.0f;
			vel.y = nudge;
			velocityChanged = true;
		}

		if (velocityChanged || std::abs(speed - m_Speed) > 0.5f)
		{
			vel = MathAPI::Normalize(vel) * m_Speed;
			Physics2DAPI::SetLinearVelocity(GetEntity(), vel);
		}
	}

	void BallController::OnDestroy()
	{
		// Cleanup
	}

	void BallController::OnCollisionBegin(const CollisionContact& contact)
	{
		Entity other = contact.otherEntity;

		if (other.Tag() == "Player")
		{
			float diagSpeed = m_Speed * 0.707f;
			Vec3 ballPos = GameplayAPI::GetPosition(GetEntity());
			Vec3 paddlePos = GameplayAPI::GetPosition(other);

			float side = (ballPos.x > paddlePos.x) ? 1.0f : -1.0f;

			Vec2 newVel(diagSpeed * side, std::abs(diagSpeed)); 
			Physics2DAPI::SetLinearVelocity(GetEntity(), newVel);

			if (m_PaddleHitSound) SoundAPI::PlaySound(m_PaddleHitSound, 0.5f);
			GameplayAPI::ShakeCamera(hitParams);
		}
		else if (other.Tag() == "Brick" || other.Tag() == "Wall")
		{
			if (other.Tag() == "Brick")
			{
				if (m_BrickHitSound) SoundAPI::PlaySound(m_BrickHitSound, 0.6f);
			}
			else if (other.Tag() == "Wall")
			{
				if (m_WallHitSound) SoundAPI::PlaySound(m_WallHitSound, 0.3f);
			}

			Vec2 velocity = Physics2DAPI::GetLinearVelocity(GetEntity());
			if (MathAPI::Length(velocity) > 0.01f) 
			{
				Vec2 normalized = MathAPI::Normalize(velocity);
				Physics2DAPI::SetLinearVelocity(GetEntity(), normalized * m_Speed);
			}
		}
	}

	void BallController::OnCollisionEnd(const CollisionContact& contact)
	{
		// Not used
	}

	void BallController::OnCollisionHit(const CollisionContact& contact)
	{
		// Not used
	}

	void BallController::Launch()
	{
		m_IsLaunched = true;

		bool goLeft = MathAPI::RandomBool(0.5f);
		float diagSpeed = m_Speed * 0.707f;

		Vec2 velocity(
			diagSpeed * (goLeft ? -1.0f : 1.0f),
			-diagSpeed
		);

		Physics2DAPI::SetLinearVelocity(GetEntity(), velocity);
	}

	void BallController::Reset()
	{
		m_IsLaunched = false;
		Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0, 0));
	}
}