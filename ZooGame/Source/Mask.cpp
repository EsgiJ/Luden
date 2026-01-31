#include "Mask.h"

#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void Mask::OnCreate()
    {
        m_ElephantIdleAnim = GetResource<Animation>("ElephantIdleAnim");
        m_ElephantSideAnim = GetResource<Animation>("ElephantSideAnim");
        m_ElephantBackAnim = GetResource<Animation>("ElephantBackAnim");
        m_ElephantFrontAnim = GetResource<Animation>("ElephantFrontAnim");

        m_RabbitIdleAnim = GetResource<Animation>("RabbitIdleAnim");
        m_RabbitSideAnim = GetResource<Animation>("RabbitSideAnim");
        m_RabbitBackAnim = GetResource<Animation>("RabbitBackAnim");
        m_RabbitFrontAnim = GetResource<Animation>("RabbitFrontAnim");

        m_MonkeyIdleAnim = GetResource<Animation>("MonkeyIdleAnim");
        m_MonkeySideAnim = GetResource<Animation>("MonkeySideAnim");
        m_MonkeyBackAnim = GetResource<Animation>("MonkeyBackAnim");
        m_MonkeyFrontAnim = GetResource<Animation>("MonkeyFrontAnim");
    }

    void Mask::OnUpdate(TimeStep ts)
    {
	    if (m_Type == MaskType::None)
	    {
            m_CurrentIdleAnim = nullptr;
            m_CurrentBackAnim = nullptr;
            m_CurrentFrontAnim = nullptr;
            m_CurrentSideAnim = nullptr;
	    }
        else if (m_Type == MaskType::Elephant)
        {
            m_CurrentIdleAnim = m_ElephantIdleAnim;
            m_CurrentBackAnim = m_ElephantBackAnim;
            m_CurrentFrontAnim = m_ElephantFrontAnim;
            m_CurrentSideAnim = m_ElephantSideAnim;
        }
        else if (m_Type == MaskType::Monkey)
        {
            m_CurrentIdleAnim = m_MonkeyIdleAnim;
            m_CurrentBackAnim = m_MonkeyBackAnim;
            m_CurrentFrontAnim = m_MonkeyFrontAnim;
            m_CurrentSideAnim = m_MonkeySideAnim;
        }
        else if (m_Type == MaskType::Rabbit)
        {
            m_CurrentIdleAnim = m_RabbitIdleAnim;
            m_CurrentBackAnim = m_RabbitBackAnim;
            m_CurrentFrontAnim = m_RabbitFrontAnim;
            m_CurrentSideAnim = m_RabbitSideAnim;
        }
    }

    void Mask::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Mask::OnCollisionBegin(const CollisionContact& contact)
    {
    }

    void Mask::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Mask::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
