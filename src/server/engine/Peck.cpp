#include "Peck.h"

#include "DamageField.h"
#include "FieldHandler.h"
#include "FireRateReset.h"

#ifndef BULLET_PHYSICS
#define BULLET_PHYSICS
#include <BulletPhysics\btBulletDynamicsCommon.h>
#include <BulletPhysics\btBulletCollisionCommon.h>
#endif

Peck::Peck(btDiscreteDynamicsWorld* curworld) : Weapon(m_meleeFireRate, m_meleeDamage, curworld)
{
    m_nextFireTick = 0;
}

Peck::~Peck()
{
}

int Peck::UseWeapon(const btVector3& position,
                    const btMatrix3x3& rotation,
                    int playerid,
                    int teamid,
                    Entity* owner)
{
    if (m_fireFlag)
    {
        // Spawns damage field for peck attack into the world
        /*
        btVector3 vec = btVector3(0, 0, 3);
        vec = ((vec) * (*rotation));
        vec = ((vec) + (*position));
        */
        btVector3 vec = rotation * btVector3(0, 0, 3) + position;

        btVector3 fieldPos = btVector3(vec.getX(), vec.getY(), vec.getZ());
        btCollisionShape* peckSphere = new btSphereShape(btScalar(3));
        DamageField* peckField =
            new DamageField(1, m_meleeDamage, fieldPos, peckSphere, teamid, m_curWorld);

        // TODO: Add to damage field checker. Add this new field to the checker
        FieldHandler::Instance()->AddField(peckField);

        m_fireFlag = 0;
        m_nextFireTick = FireRateReset::Instance()->m_currentWorldTick + m_meleeFireRate;

        // add used weapon to "used" list in FireRateReset static object
        FireRateReset::Instance()->AddWeapon(this);
    }
    return 1; // infinite ammo
}

void Peck::ReloadWeapon()
{
}
