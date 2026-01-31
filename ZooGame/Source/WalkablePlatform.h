#pragma once
#include "Luden.h"

namespace Luden
{
    class WalkablePlatform : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        Vec2 PlatformSize = Vec2(200.0f, 50.0f);

    private:
        bool IsPlayerOnPlatform();
        bool m_PlayerWasOnPlatform = false;
    };
}