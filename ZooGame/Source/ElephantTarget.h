#pragma once
#include "Luden.h"

namespace Luden
{
    class ElephantTarget : public ScriptableEntity
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
        void Toggle();

    public:
        Vec3 StartPosition;
        Vec3 EndPosition;
        float DistanceBetweenPositions = 400.0f;
        float MoveSpeed = 2.0f;
        bool IsAtEnd = false;
        bool IsActivated = false;
        bool m_FirstColliderSpawned = false;
    private:
        void MoveTowardsEnd(TimeStep ts);
        void MoveTowardsStart(TimeStep ts);
        void SpawnColliderAtCurrentPosition();
        void DestroyCollider();

        Entity m_ElephantEntity;
        float m_ColliderDistanceFromElephant = 100.0f;

        Entity m_ColliderEntity;
        Vec2 m_ColliderSize = Vec2(500.0f, 500.0f); 
    };
}