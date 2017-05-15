#include "BlastMine.h"

#include "BlastField.h"
#include "EntitySpawner.h"
#include "FireRateReset.h"
#include "MineBulletHandler.h"

BlastMine::BlastMine(btDiscreteDynamicsWorld* curworld) : Weapon(curworld)
{
    m_type = WeaponType::BlastMine;
    m_nextFireTick = 0;
    m_fireRate = 50;
    m_damage = 6;
    m_currentAmmo = 4;
    m_maxAmmo = 4;
    m_gunSpeed = btVector3(0, 0, 0);
}

BlastMine::~BlastMine()
{
}

int BlastMine::UseWeapon(const btVector3& position,
                         const btMatrix3x3& rotation,
                         int playerid,
                         int teamid,
                         Entity* owner)
{
    if (m_fireFlag)
    {
        btCollisionShape* bulletShape = new btCylinderShape(btVector3(2, 1, 2));

        // Create bullet physics object
        btVector3 globalPos = rotation * btVector3(0, 1, 9) + position;

        btDefaultMotionState* bulletMotionState =
            new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), (globalPos)));
        btScalar mass = 1;
        btVector3 bulletInertia(0, 0, 0);
        bulletShape->calculateLocalInertia(mass, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI(
            mass, bulletMotionState, bulletShape, bulletInertia);
        btRigidBody* bRigidBody = new btRigidBody(bulletRigidBodyCI);

        bRigidBody->setLinearFactor(btVector3(0, 0, 0));

        // set bullet rotation
        btTransform currentTrans;
        bRigidBody->getMotionState()->getWorldTransform(currentTrans);
        currentTrans.setBasis(rotation);
        bRigidBody->getMotionState()->setWorldTransform(currentTrans);
        bRigidBody->setCenterOfMassTransform(currentTrans);

        // set bullet velocity
        btVector3 newVelocity = rotation * m_gunSpeed;

        bRigidBody->setLinearVelocity(newVelocity);
        bRigidBody->forceActivationState(DISABLE_DEACTIVATION);

        btCollisionShape* expSphere = new btSphereShape(btScalar(m_explosionSize));
        btVector3 origin = btVector3(0, 0, 0); // field origin determined later by the handler
        BlastField* field = new BlastField(
            m_nonFriendly, m_stun, m_magnitude, m_ttl, m_damage, origin, expSphere, teamid, m_curWorld);

        // use the simple handler for the seedgun
        MineBulletHandler* handler = new MineBulletHandler(field);

        // Spawns bullet with this gun's damage, speed, and necessary ids into world
        Bullet* fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler, bRigidBody, m_curWorld);
        handler->SetBullet(fireProjectile);

        m_fireFlag = 0;
        m_nextFireTick = FireRateReset::Instance()->m_currentWorldTick + m_fireRate;

        // add used weapon to "used" list in FireRateReset static object
        FireRateReset::Instance()->AddWeapon(this);
        m_currentAmmo--;
    }
    return m_currentAmmo;
}