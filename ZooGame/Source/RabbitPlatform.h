#pragma once
#include "Luden.h"

namespace Luden
{
    class MonkeyArm;

    class RabbitPlatform : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        Vec3 GetLandingPosition();

        Entity GetMonkeyInDirection(int direction);  

    public:
        Vec2 PlatformSize = Vec2(200.0f, 50.0f);

        Vec3 LandingOffset = Vec3(0.0f, -30.0f, 0.0f);

        Entity MonkeyArmUp;
        Entity MonkeyArmDown;
        Entity MonkeyArmLeft;
        Entity MonkeyArmRight;

    private:
        bool IsPlayerOnPlatform();
        bool m_PlayerWasOnPlatform = false;
    };
}