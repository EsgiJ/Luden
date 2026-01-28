#include "Bullet.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void Bullet::OnCreate()
    {
        if (m_OwnerTag == "Player")
            GetEntity().SetTag("PlayerBullet");
        else
            GetEntity().SetTag("EnemyBullet");
    }

    void Bullet::OnUpdate(TimeStep ts)
    {
        m_TimeAlive += ts;

        if (m_TimeAlive >= m_Lifetime)
        {
            GameplayAPI::DestroyEntity(GetEntity());
        }

        if (!GameplayAPI::IsOnScreen(GetEntity()))
        {
            GameplayAPI::DestroyEntity(GetEntity());
        }
    }

    void Bullet::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Bullet::OnCollisionBegin(const CollisionContact& contact)
    {
        Entity other = contact.otherEntity;

        std::cout << "Bullet hit. OwnerTag: " << m_OwnerTag << " OtherTag: " << other.Tag() << std::endl;

        if (m_OwnerTag == "Player" && other.Tag() == "Enemy")
        {
            std::cout << "Bullet hit Player" << std::endl;
            GameplayAPI::DestroyEntity(GetEntity());
        }

        if (m_OwnerTag == "Enemy" && other.Tag() == "Player")
        {
            std::cout << "Bullet hit Enemy" << std::endl;
            GameplayAPI::DestroyEntity(GetEntity());
        }
    }

    void Bullet::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Bullet::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
