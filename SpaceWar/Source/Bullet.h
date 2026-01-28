#pragma once
#include "Luden.h"

namespace Luden
{
	class Bullet : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        float m_Lifetime = 5.0f;
        int m_Damage = 1;
        String m_OwnerTag = "Player";  

    private:
        float m_TimeAlive = 0.0f;
    };
}
