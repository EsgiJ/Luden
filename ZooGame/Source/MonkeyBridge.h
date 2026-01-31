#pragma once
#include "Luden.h"

namespace Luden
{
	class MonkeyBridge : public ScriptableEntity
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

	public:
        float DetectionRadius = 500.0f;

        float TargetRotation = 90.0f;    
        float StartRotation = 0.0f;      
        float RotateSpeed = 2.0f;        

        bool IsActivated = false;
        bool IsRotated = false;
	private:
        void RotateToTarget(TimeStep ts);
        void RotateToStart(TimeStep ts);
    };
}
