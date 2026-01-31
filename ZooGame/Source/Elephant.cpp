#include "Elephant.h"

#include <ScriptAPI/AnimationAPI.h>

namespace Luden
{
    void Elephant::OnCreate()
    {
        m_EmptyAnim = GetResource<Animation>("EmptyAnim");
        m_ElephantPushAnim = GetResource<Animation>("ElephantPushAnim");
        m_ElephantPullAnim = GetResource<Animation>("ElephantPullAnim");

        AnimationAPI::PlayAnimation(GetEntity(), m_EmptyAnim);
    }

    void Elephant::OnUpdate(TimeStep ts)
    {
	    if (m_AnimTriggered)
	    {
            m_PassedTime += ts;
	    }

	    if (m_AnimTriggered && m_PassedTime >= m_AnimDuration)
	    {
            Deactivate();
	    }
    }

    void Elephant::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Elephant::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void Elephant::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Elephant::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void Elephant::Activate()
    {
	    if (m_ElephantPushAnim)
	    {
            AnimationAPI::PlayAnimation(GetEntity(), m_ElephantPushAnim);
            m_AnimTriggered = true;
            m_PassedTime = 0.0f;
	    }
    }

    void Elephant::Deactivate()
    {
        if (m_ElephantPushAnim)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_ElephantPullAnim);
            m_AnimTriggered = false;
        }
    }

}
