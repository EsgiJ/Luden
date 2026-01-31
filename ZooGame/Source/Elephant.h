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

        void Activate();
        void Deactivate();

        float m_AnimDuration = 1.2f;
        float m_PassedTime = 0.0f;

        bool m_AnimTriggered = false;
    };
}
