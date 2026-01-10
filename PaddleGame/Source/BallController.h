#pragma once
#include "Luden.h"

namespace Luden
{
	class BallController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

		void Launch();
		void Reset();

	private:
		float m_Speed = 8.0f;
		bool m_IsLaunched = false;
		Entity m_Paddle;

		SoundRef m_PaddleHitSound;
		SoundRef m_BrickHitSound;
		SoundRef m_WallHitSound;
    };
}
