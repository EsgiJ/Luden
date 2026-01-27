#include "HealthBar.h"

#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void HealthBar::OnCreate()
    {
        m_HeartPrefab = GetResource<Prefab>("Heart");

        auto& healthBarComponent = GetEntity().Get<HealthComponent>();
        CreateHearts(healthBarComponent.max);
    }

    void HealthBar::OnUpdate(TimeStep ts)
    {
        if (!GetEntity().Has<HealthComponent>())
            return;

        auto& health = GetEntity().Get<HealthComponent>();

        if (health.current != m_LastKnownHealth)
        {
            UpdateHealthBar();
            m_LastKnownHealth = health.current;
        }

        UpdateHeartPositions();
    }

    void HealthBar::OnDestroy()
    {
        // TODO: Cleanup
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
        if (!GetEntity().Has<HealthComponent>())
            return;

        auto& health = GetEntity().Get<HealthComponent>();

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

            Entity ownerEntity = GetEntity();
            Entity heart;
            if (ownerEntity.IsValid())
				 heart = GameplayAPI::SpawnPrefabAsChild(m_HeartPrefab, ownerEntity, heartPosition);



            if (heart.IsValid())
            {
                m_HeartEntities.push_back(heart);
            }
        }
    }

    void HealthBar::UpdateHeartPositions()
    {
        if (!GetEntity().Has<HealthComponent>())
            return;

        auto& health = GetEntity().Get<HealthComponent>();
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
        Vec3 ownerPosition = GameplayAPI::GetPosition(GetEntity());
        float totalWidth = (count - 1) * m_HeartSpacing;

        float startX = ownerPosition.x - (totalWidth / 2);

        Vec3 heartPosition;
        heartPosition.x = startX + (index * m_HeartSpacing);
        heartPosition.y = ownerPosition.y + m_Offset.y;
        heartPosition.z = ownerPosition.z;
        
    	return heartPosition;
    }
}
