#pragma once
#include "Luden.h"

namespace Luden
{
    enum class MaskType : uint8_t
    {
        None,
	    Elephant,
        Rabbit,
        Monkey
    };
	class Mask : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        MaskType m_Type = MaskType::None;

        AnimationRef m_CurrentIdleAnim = nullptr;
        AnimationRef m_CurrentBackAnim = nullptr;
        AnimationRef m_CurrentFrontAnim = nullptr;
        AnimationRef m_CurrentSideAnim = nullptr;

        AnimationRef m_ElephantIdleAnim = nullptr;
        AnimationRef m_ElephantBackAnim = nullptr;
        AnimationRef m_ElephantFrontAnim = nullptr;
        AnimationRef m_ElephantSideAnim = nullptr;

        AnimationRef m_RabbitIdleAnim = nullptr;
        AnimationRef m_RabbitBackAnim = nullptr;
        AnimationRef m_RabbitFrontAnim = nullptr;
        AnimationRef m_RabbitSideAnim = nullptr;

        AnimationRef m_MonkeyIdleAnim = nullptr;
        AnimationRef m_MonkeyBackAnim = nullptr;
        AnimationRef m_MonkeyFrontAnim = nullptr;
        AnimationRef m_MonkeySideAnim = nullptr;
    };
}
