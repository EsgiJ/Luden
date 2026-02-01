#include "Elephant.h"

#include <ScriptAPI/AnimationAPI.h>

namespace Luden
{
    void Elephant::OnCreate()
    {
        m_EmptyAnim = GetResource<Animation>("ElephantIdleAnim");
        m_ElephantPushAnim = GetResource<Animation>("ElephantPushAnim");
        m_ElephantPullAnim = GetResource<Animation>("ElephantPullAnim");
        m_ElephantSquirtAnim = GetResource<Animation>("ElephantSquirtAnim");

        AnimationAPI::PlayAnimation(GetEntity(), m_EmptyAnim);
    }

    void Elephant::OnUpdate(TimeStep ts)
    {
	    if (AnimTriggered)
	    {
            PassedTime += ts;
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
            AnimTriggered = true;
            PassedTime = 0.0f;
	    }
    }

    void Elephant::Deactivate()
    {
        if (m_ElephantPushAnim)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_ElephantPullAnim);
            AnimTriggered = false;
            IsActivated = false;
        }
    }

    void Elephant::Toggle()
    {
	    if (!IsActivated)
	    {
            Activate();
	    }
	    else
	    {
            Deactivate();
	    }
    }
}
