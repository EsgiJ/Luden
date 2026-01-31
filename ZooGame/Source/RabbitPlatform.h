#pragma once
#include "Luden.h"

namespace Luden
{
	class RabbitPlatform : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        void TeleportPlayerHere(Entity player);
	public:
        float DetectionRadius = 500.0f;

        float TeleportSpeed = 5.0f;  
        Vec3 TeleportOffset = Vec3(0.0f, 0.0f, 0.0f);  

    private:
        bool m_IsTeleporting = false;
        Entity m_TeleportingPlayer;
        Vec3 m_TargetPosition;
    };
}
