#pragma once
#include "Luden.h"

namespace Luden
{
	class Elephant : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        //ElephantAnim
        AnimationRef m_ElephantPushAnim;
        AnimationRef m_ElephantPullAnim;
        AnimationRef m_EmptyAnim;
        AnimationRef m_ElephantSquirtAnim;
        void Activate();
        void Deactivate();
        void Toggle();

        bool IsActivated = false;
        Vec2 m_StartPosition;

		float AnimDuration = 5.0f;
        float PassedTime = 0.0f;

        bool AnimTriggered = false;

        Entity m_ElephantTarget;
    };
}
