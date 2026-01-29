#include "HealthBar.h"

#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void HealthBar::OnCreate()
    {
        m_HeartPrefab = GetResource<Prefab>("Heart");

        Entity parentEntity = GetEntity().GetParent();

        if (!parentEntity.IsValid())
            return;

        if (!parentEntity.Has<HealthComponent>())
            return;

        auto& health = parentEntity.Get<HealthComponent>();
        CreateHearts(health.max);
        m_LastKnownHealth = health.current;
    }

    void HealthBar::OnUpdate(TimeStep ts)
    {
        Entity parentEntity = GetEntity().GetParent();

        if (!parentEntity.IsValid() || !parentEntity.Has<HealthComponent>())
            return;

        auto& health = parentEntity.Get<HealthComponent>();

        if (health.current != m_LastKnownHealth)
        {
            UpdateHealthBar();
            m_LastKnownHealth = health.current;
        }

        UpdateHeartPositions();
    }

    void HealthBar::OnDestroy()
    {
        DestroyAllHearts();
    }

    void HealthBar::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void HealthBar::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void HealthBar::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void HealthBar::DestroyAllHearts()
    {
        for (auto& heart : m_HeartEntities)
        {
            if (heart.IsValid())
            {
                GameplayAPI::DestroyEntity(heart);
            }
        }
        m_HeartEntities.clear();
    }

    void HealthBar::UpdateHealthBar()
    {
        Entity parentEntity = GetEntity().GetParent();

        if (!parentEntity.IsValid() || !parentEntity.Has<HealthComponent>())
            return;

        auto& health = parentEntity.Get<HealthComponent>();

        DestroyAllHearts();
        CreateHearts(health.current);
        m_LastKnownHealth = health.current;
    }

    void HealthBar::CreateHearts(int count)
    {
        if (m_HeartPrefab == nullptr)
            return;

        for (int i = 0; i < count; i++)
        {
            Vec3 heartPosition = CalculateHeartPosition(i, count);

            Entity healthBarEntity = GetEntity();

            if (!healthBarEntity.IsValid())
                continue;

            Entity heart = GameplayAPI::SpawnPrefabAsChild(m_HeartPrefab, healthBarEntity, heartPosition);



            if (heart.IsValid())
            {
                m_HeartEntities.push_back(heart);
            }
        }
    }

    void HealthBar::UpdateHeartPositions()
    {
        Entity parentEntity = GetEntity().GetParent();

        if (!parentEntity.IsValid() || !parentEntity.Has<HealthComponent>())
            return;

        auto& health = parentEntity.Get<HealthComponent>();
        int count = health.current;

        for (int i = 0; i < m_HeartEntities.size() && i < count; i++)
        {
            Entity heart = m_HeartEntities[i];
            if (heart.IsValid())
            {
                Vec3 newPos = CalculateHeartPosition(i, count);
                GameplayAPI::SetPosition(heart, newPos);
            }
        }
    }

    Vec3 HealthBar::CalculateHeartPosition(int index, int count)
    {
        Entity parentEntity = GetEntity().GetParent();

        if (!parentEntity.IsValid())
            return Vec3(0.0f, 0.0f, 0.0f);

        float totalWidth = (count - 1) * m_HeartSpacing;
        float startX = -(totalWidth / 2.0f);

        Vec3 heartPosition;
        heartPosition.x = startX + (index * m_HeartSpacing);
        heartPosition.y = m_Offset.y;
        heartPosition.z = 0.0f;

        return heartPosition;
    }
}
