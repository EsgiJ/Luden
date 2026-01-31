#include "Mask.h"
#include <iostream>
#include <ScriptAPI/GameplayAPI.h>
#include <ScriptAPI/AnimationAPI.h>

namespace Luden
{
    void Mask::OnCreate()
    {
        m_EmptyAnim = GetResource<Animation>("EmptyAnim");

        m_ElephantIdleAnim = GetResource<Animation>("ElephantMaskIdle");
        m_ElephantSideAnim = GetResource<Animation>("ElephantMaskSide");
        m_ElephantFrontAnim = GetResource<Animation>("ElephantMaskFront");

        m_RabbitIdleAnim = GetResource<Animation>("RabbitMaskIdleAnim");
        m_RabbitSideAnim = GetResource<Animation>("RabbitMaskSideAnim");
        m_RabbitFrontAnim = GetResource<Animation>("RabbitMaskFrontAnim");

        m_MonkeyIdleAnim = GetResource<Animation>("MonkeyMaskIdleAnim");
        m_MonkeySideAnim = GetResource<Animation>("MonkeyMaskSideAnim");
        m_MonkeyFrontAnim = GetResource<Animation>("MonkeyMaskFrontAnim");

        if (m_EmptyAnim)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_EmptyAnim);
        }
    }

    void Mask::OnUpdate(TimeStep ts)
    {
        static MaskType lastType = MaskType::None;

        if (m_Type != lastType)
        {
            UpdateMaskAnimations();
            lastType = m_Type;
        }
    }

    void Mask::UpdateMaskAnimations()
    {
        if (m_Type == MaskType::None)
        {
            m_CurrentIdleAnim = nullptr;
            m_CurrentBackAnim = nullptr;
            m_CurrentFrontAnim = nullptr;
            m_CurrentSideAnim = nullptr;

            if (m_EmptyAnim)
            {
                AnimationAPI::PlayAnimation(GetEntity(), m_EmptyAnim);
                std::cout << "[Mask] Changed to None - Playing EmptyAnim" << std::endl;
            }
        }
        else if (m_Type == MaskType::Elephant)
        {
            m_CurrentIdleAnim = m_ElephantIdleAnim;
            m_CurrentBackAnim = m_ElephantBackAnim;
            m_CurrentFrontAnim = m_ElephantFrontAnim;
            m_CurrentSideAnim = m_ElephantSideAnim;

            if (m_CurrentIdleAnim)
            {
                AnimationAPI::PlayAnimation(GetEntity(), m_CurrentIdleAnim);
                std::cout << "[Mask] Changed to Elephant" << std::endl;
            }
        }
        else if (m_Type == MaskType::Monkey)
        {
            m_CurrentIdleAnim = m_MonkeyIdleAnim;
            m_CurrentBackAnim = m_MonkeyBackAnim;
            m_CurrentFrontAnim = m_MonkeyFrontAnim;
            m_CurrentSideAnim = m_MonkeySideAnim;

            if (m_CurrentIdleAnim)
            {
                AnimationAPI::PlayAnimation(GetEntity(), m_CurrentIdleAnim);
                std::cout << "[Mask] Changed to Monkey" << std::endl;
            }
        }
        else if (m_Type == MaskType::Rabbit)
        {
            m_CurrentIdleAnim = m_RabbitIdleAnim;
            m_CurrentBackAnim = m_RabbitBackAnim;
            m_CurrentFrontAnim = m_RabbitFrontAnim;
            m_CurrentSideAnim = m_RabbitSideAnim;

            if (m_CurrentIdleAnim)
            {
                AnimationAPI::PlayAnimation(GetEntity(), m_CurrentIdleAnim);
                std::cout << "[Mask] Changed to Rabbit" << std::endl;
            }
        }
    }

    void Mask::OnDestroy()
    {
        // Cleanup
    }

    void Mask::OnCollisionBegin(const CollisionContact& contact)
    {
    }

    void Mask::OnCollisionEnd(const CollisionContact& contact)
    {
    }

    void Mask::OnCollisionHit(const CollisionContact& contact)
    {
    }
}