#include "MonkeyArm.h"
#include <iostream>
#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void MonkeyArm::OnCreate()
    {
        auto children = GameplayAPI::GetChildren(GetEntity());

        for (auto child : children)
        {
            if (child.Tag() == "MonkeyHead")
            {
                m_HeadEntity = child;
            }
            else if (child.Tag() == "MonkeyArm_Sprite")
            {
                m_ArmEntity = child;
                auto& spriteRenderer = m_ArmEntity.Get<SpriteRendererComponent>();
                spriteRenderer.tint.a = 0.0f;
            }
        }

        std::cout << "[MonkeyArm] Created - Direction: " << (int)Direction << std::endl;
    }

    void MonkeyArm::OnUpdate(TimeStep ts)
    {
      
    }

    void MonkeyArm::OnDestroy()
    {
        // Cleanup
    }

    void MonkeyArm::OnCollisionBegin(const CollisionContact& contact)
    {
      
    }

    void MonkeyArm::OnCollisionEnd(const CollisionContact& contact)
    {
      
    }

    void MonkeyArm::OnCollisionHit(const CollisionContact& contact)
    {
        
    }

    void MonkeyArm::Activate()
    {
        std::cout << "[MonkeyArm] Activated" << std::endl;
        if (IsActivated)
            return;

        IsActivated = true;

        if (m_ArmEntity.IsValid())
        {
            auto& spriteRenderer = m_ArmEntity.Get<SpriteRendererComponent>();
            spriteRenderer.tint.a = 255.0f;
            std::cout << "[MonkeyArm] Arm extended!" << std::endl;
        }

        //TODO: Play extend animation/sound
    }

    void MonkeyArm::Deactivate()
    {
        if (!IsActivated)
            return;

        IsActivated = false;

        if (m_ArmEntity.IsValid())
        {
            auto& spriteRenderer = m_ArmEntity.Get<SpriteRendererComponent>();
            spriteRenderer.tint.a = 0.0f;
            std::cout << "[MonkeyArm] Arm retracted!" << std::endl;
        }
    }

    Vec3 MonkeyArm::GetTargetPosition()
    {
        Vec3 monkeyPos = GameplayAPI::GetPosition(GetEntity());

        Vec3 offset(0.0f, 0.0f, 0.0f);

        switch (Direction)
        {
        case MonkeyDirection::Up:
            offset.y = -WalkDistance;  
            break;
        case MonkeyDirection::Down:
            offset.y = WalkDistance;   
            break;
        case MonkeyDirection::Left:
            offset.x = -WalkDistance;
            break;
        case MonkeyDirection::Right:
            offset.x = WalkDistance;
            break;
        }

        return monkeyPos + offset;
    }
}