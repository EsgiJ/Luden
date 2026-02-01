#include "ElephantTarget.h"
#include <iostream>
#include <ScriptAPI/GameplayAPI.h>
#include <ScriptAPI/MathAPI.h>

namespace Luden
{
    void ElephantTarget::OnCreate()
    {
        std::cout << "[ElephantTarget] Created - waiting for Elephant to set positions" << std::endl;
    }

    void ElephantTarget::OnUpdate(TimeStep ts)
    {
        if (!m_FirstColliderSpawned && StartPosition != Vec3(0.0f, 0.0f, 0.0f))
        {
            SpawnColliderAtCurrentPosition();
            m_FirstColliderSpawned = true;
        }

        if (IsActivated)
        {
            MoveTowardsEnd(ts);
        }
        else if (IsAtEnd)
        {
            MoveTowardsStart(ts);
        }
    }

    void ElephantTarget::OnDestroy()
    {
        DestroyCollider();
    }

    void ElephantTarget::OnCollisionBegin(const CollisionContact& contact) {}
    void ElephantTarget::OnCollisionEnd(const CollisionContact& contact) {}
    void ElephantTarget::OnCollisionHit(const CollisionContact& contact) {}

    void ElephantTarget::Toggle()
    {
        std::cout << "[ElephantTarget] Toggle - Start: (" << StartPosition.x << "," << StartPosition.y
            << ") End: (" << EndPosition.x << "," << EndPosition.y << ")" << std::endl;

        if (IsActivated)
            Deactivate();
        else
            Activate();
    }

    void ElephantTarget::Activate()
    {
        if (IsActivated)
        {
            std::cout << "[ElephantTarget] Already activated!" << std::endl;
            return;
        }

        IsActivated = true;
        DestroyCollider();

        std::cout << "[ElephantTarget] Activated! Moving to end position" << std::endl;
    }

    void ElephantTarget::Deactivate()
    {
        if (!IsActivated)
        {
            std::cout << "[ElephantTarget] Already deactivated!" << std::endl;
            return;
        }

        IsActivated = false;
        SpawnColliderAtCurrentPosition();

        std::cout << "[ElephantTarget] Deactivated! Collider spawned" << std::endl;
    }

    void ElephantTarget::MoveTowardsEnd(TimeStep ts)
    {
        Vec3 currentPos = GameplayAPI::GetPosition(GetEntity());
        Vec3 newPos = MathAPI::Lerp(currentPos, EndPosition, MoveSpeed * ts);
        GameplayAPI::SetPosition(GetEntity(), newPos);

        float dist = GameplayAPI::Distance(newPos, EndPosition);
        if (dist < 1.0f)
        {
            GameplayAPI::SetPosition(GetEntity(), EndPosition);
            IsAtEnd = true;

            SpawnColliderAtCurrentPosition();
        }
    }

    void ElephantTarget::MoveTowardsStart(TimeStep ts)
    {
        if (m_ColliderEntity.IsValid())
        {
            DestroyCollider();
        }

        Vec3 currentPos = GameplayAPI::GetPosition(GetEntity());
        Vec3 newPos = MathAPI::Lerp(currentPos, StartPosition, MoveSpeed * ts);
        GameplayAPI::SetPosition(GetEntity(), newPos);

        float dist = GameplayAPI::Distance(newPos, StartPosition);
        if (dist < 1.0f)
        {
            GameplayAPI::SetPosition(GetEntity(), StartPosition);
            IsAtEnd = false;

            SpawnColliderAtCurrentPosition();
        }
    }

    void ElephantTarget::SpawnColliderAtCurrentPosition()
    {
        DestroyCollider();

        Vec3 spawnPos;

        if (IsAtEnd)
        {
            spawnPos = EndPosition;
        }
        else
        {
            spawnPos = StartPosition;
        }

        m_ColliderEntity = GameplayAPI::SpawnEntity("ElephantTargetCollider", spawnPos);

        if (!m_ColliderEntity.IsValid())
        {
            std::cout << "[ElephantTarget] Failed to spawn collider entity!" << std::endl;
            return;
        }

        auto& rb = m_ColliderEntity.Add<RigidBody2DComponent>();
        rb.BodyType = RigidBody2DComponent::Type::Static;
        rb.FixedRotation = true;

        auto& collider = m_ColliderEntity.Add<BoxCollider2DComponent>();
        collider.Size = m_ColliderSize;
        collider.Offset = Vec2(0.0f, 0.0f);

        GameplayAPI::GetCurrentScene()->GetPhysicsManager().RegisterEntity(m_ColliderEntity);

        std::cout << "[ElephantTarget] Collider spawned at: " << spawnPos.x << ", " << spawnPos.y << std::endl;
    }

    void ElephantTarget::DestroyCollider()
    {
        if (m_ColliderEntity.IsValid())
        {
            GameplayAPI::DestroyEntity(m_ColliderEntity);
            m_ColliderEntity = Entity();
            std::cout << "[ElephantTarget] Collider destroyed" << std::endl;
        }
    }
}