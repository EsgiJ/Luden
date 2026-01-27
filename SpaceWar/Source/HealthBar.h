#pragma once
#include "Luden.h"

namespace Luden
{
	class HealthBar : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;
    public:

        PrefabRef m_HeartPrefab;
        Vec2 m_Offset = { 0.0f, -50.0f };
        float m_HeartSpacing = 60.0f;

    private:
        void UpdateHealthBar();

        void CreateHearts(int count);
        void UpdateHeartPositions();
		Vec3 CalculateHeartPosition(int index, int count);
        void DestroyAllHearts();
    private:
        Vector<Entity> m_HeartEntities;
        int m_LastKnownHealth = -1;
    };
}
