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
	public:
        float DetectionRadius = 1000.0f;

        Vec3 StartPosition;
        Vec3 EndPosition;
        float DistanceBetweenPositions = 400.0f;
        float MoveSpeed = 2.0f;
        bool IsAtEnd = false;

        bool IsActivated = false;
        float ActiveDuration = 5.0f;  
        float TimeActive = 0.0f;
	private:
        void MoveTowardsEnd(TimeStep ts);
        void MoveTowardsStart(TimeStep ts);

        Vec3 m_CurrentTarget;
    };
}
