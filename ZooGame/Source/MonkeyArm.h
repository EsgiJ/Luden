#pragma once
#include "Luden.h"

namespace Luden
{
    enum class MonkeyDirection : uint8_t
    {
        Up = 0,
        Down = 1,
        Left = 2,
        Right = 3
    };

    class MonkeyArm : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        void Activate();
        void Deactivate();

        Vec3 GetTargetPosition();

    public:
        MonkeyDirection Direction = MonkeyDirection::Right;

        bool IsActivated = false;

        float WalkDistance = 200.0f;

    private:
        Entity m_HeadEntity;   
        Entity m_ArmEntity;    
    };
}