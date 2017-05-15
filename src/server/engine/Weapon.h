#pragma once
#include "network/GameData.h"
#include <BulletPhysics/LinearMath/btVector3.h>

class Entity;
class btDiscreteDynamicsWorld;
class btMatrix3x3;

class Weapon
{
protected:
    int m_fireRate;
    int m_damage;
    int m_currentAmmo; // Current ammo of this weapon
    int m_maxAmmo;     // Capacity of this weapon
    btVector3 m_gunSpeed;

    WeaponType m_type;

    btDiscreteDynamicsWorld* m_curWorld;

public:
    unsigned int m_nextFireTick;
    int m_fireFlag = 1;

    Weapon(btDiscreteDynamicsWorld* curworld);
    Weapon(int firerate, int wdamage, btDiscreteDynamicsWorld* curworld);
    ~Weapon();

    // returns the ammo left in the weapon
    virtual int UseWeapon(const btVector3& position,
                          const btMatrix3x3& rotation,
                          int playerid,
                          int teamid,
                          Entity* owner)
    {
        return 0;
    };

    WeaponType virtual GetWeaponType() { return m_type; }

    const btVector3& GetGunSpeed() { return m_gunSpeed; }

    void virtual ReloadWeapon();
};