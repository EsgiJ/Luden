#include "Brick.h"

#include "GameManager.h"
#include "ScriptAPI/AudioAPI.h"
#include "ScriptAPI/GameplayAPI.h"
#include <iostream>

namespace Luden
{
    void Brick::OnCreate()
    {
		m_BreakSound = GetResource<Sound>("brick_break");

		UpdateColor();
    }

    void Brick::OnUpdate(TimeStep ts)
    {
		UpdateColor();
    }

    void Brick::OnDestroy()
    {
		if (m_BreakSound)
			SoundAPI::PlaySound(m_BreakSound, 0.7f);
    }

    void Brick::OnCollisionBegin(const CollisionContact& contact)
    {
		if (contact.otherEntity.Tag() == "Ball")
		{
			TakeDamage(1);
		}
	}

    void Brick::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Brick::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

	void Brick::TakeDamage(int damage)
	{
		if (!HasComponent<HealthComponent>())
			return;

		auto& health = GetComponent<HealthComponent>();
		health.current -= damage;

		if (health.current <= 0)
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
						manager->AddScore(m_ScoreValue);
					}
				}
			}

			GameplayAPI::DestroyEntity(GetEntity());
		}
		else
		{
			UpdateColor();
		}
	}

	void Brick::UpdateColor()
	{
		if (!HasComponent<HealthComponent>() || !HasComponent<SpriteRendererComponent>())
			return;

		auto& health = GetComponent<HealthComponent>();
		auto& sprite = GetComponent<SpriteRendererComponent>();

		switch (health.current)
		{
		case 3: sprite.tint = sf::Color::Red; break;
		case 2: sprite.tint = sf::Color::Yellow; break;
		case 1: sprite.tint = sf::Color::Green; break;
		default: sprite.tint = sf::Color::White; break;
		}
	}
}
