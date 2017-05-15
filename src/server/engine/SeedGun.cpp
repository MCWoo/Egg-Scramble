#include "SeedGun.h"
#include "SimpleBulletCollision.h"
#include "FireRateReset.h"

SeedGun::SeedGun(btDiscreteDynamicsWorld* curworld) : Weapon(curworld)
{
    m_type = WeaponType::SeedGun;
    m_nextFireTick = 0;
    m_fireRate = 5;
    m_damage = 20;
    m_currentAmmo = 300;
    m_maxAmmo = 300;
    m_gunSpeed = btVector3(0, 5, 80);
}

SeedGun::~SeedGun()
{
}

int SeedGun::UseWeapon(const btVector3& position,
                       const btMatrix3x3& rotation,
                       int playerid,
                       int teamid,
                       Entity* owner)
{
    if (m_fireFlag)
    {
        btCollisionShape* bulletShape = new btSphereShape(btScalar(0.5));

        // Create bullet physics object
        btVector3 globalPos = rotation * btVector3(0, 1, 10) + position;

        btDefaultMotionState* bulletMotionState =
            new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), (globalPos)));
        btScalar mass = 1;
        btVector3 bulletInertia(0, 0, 0);
        bulletShape->calculateLocalInertia(mass, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI(
            mass, bulletMotionState, bulletShape, bulletInertia);
        btRigidBody* bRigidBody = new btRigidBody(bulletRigidBodyCI);

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

        // use the simple handler for the seedgun
        SimpleBulletCollision* handler = new SimpleBulletCollision();

        // Spawns bullet with this gun's damage, speed, and necessary ids into world
        Bullet* fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler, bRigidBody, m_curWorld);

        m_fireFlag = 0;
        m_nextFireTick = FireRateReset::Instance()->m_currentWorldTick + m_fireRate;

        // add used weapon to "used" list in FireRateReset static object
        FireRateReset::Instance()->AddWeapon(this);
        m_currentAmmo--;
    }
    return m_currentAmmo;
}
