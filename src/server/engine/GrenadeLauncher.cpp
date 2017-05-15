#include "GrenadeLauncher.h"

#include "ExplosiveBulletCollision.h"
#include "FireRateReset.h"

GrenadeLauncher::GrenadeLauncher(btDiscreteDynamicsWorld* curworld) : Weapon(curworld)
{
    m_type = WeaponType::GrenadeLauncher;
    m_nextFireTick = 0;
    m_fireRate = 100;
    m_damage = 1;
    m_currentAmmo = 12;
    m_maxAmmo = 12;
    m_gunSpeed = btVector3(0, 15, 30);
}

GrenadeLauncher::~GrenadeLauncher()
{
}

int GrenadeLauncher::UseWeapon(const btVector3& position,
                               const btMatrix3x3& rotation,
                               int playerid,
                               int teamid,
                               Entity* owner)
{
    if (m_fireFlag)
    {
        btCollisionShape* grenadeShape = new btSphereShape(btScalar(0.5));

        // Create Grenade physics object
        btVector3 globalPos = rotation * btVector3(0, 5, 5) + position;

        btDefaultMotionState* grenadeMotionState =
            new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), (globalPos)));
        btScalar mass = 1;
        btVector3 grenadeInertia(0, 0, 0);
        grenadeShape->calculateLocalInertia(mass, grenadeInertia);
        btRigidBody::btRigidBodyConstructionInfo grenadeRigidBodyCI(
            mass, grenadeMotionState, grenadeShape, grenadeInertia);
        btRigidBody* bRigidBody = new btRigidBody(grenadeRigidBodyCI);

        // set Grenade rotation
        btTransform currentTrans;
        bRigidBody->getMotionState()->getWorldTransform(currentTrans);
        currentTrans.setBasis(rotation);
        bRigidBody->getMotionState()->setWorldTransform(currentTrans);
        bRigidBody->setCenterOfMassTransform(currentTrans);

        // set grenade velocity
        btVector3 newVelocity = rotation * m_gunSpeed;

        bRigidBody->setLinearVelocity(newVelocity);
        bRigidBody->forceActivationState(DISABLE_DEACTIVATION);

        ExplosiveBulletCollision* handler =
            new ExplosiveBulletCollision(EXPLOSION_SIZE, EXPLOSION_DUR);
        Bullet* fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler, bRigidBody, m_curWorld);
        handler->SetBullet(fireProjectile);

        // Spawns bullet with this gun's damage, speed, and necessary ids into world
        m_fireFlag = 0;
        m_nextFireTick = FireRateReset::Instance()->m_currentWorldTick + m_fireRate;

        // add used weapon to "used" list in FireRateReset static object
        FireRateReset::Instance()->AddWeapon(this);
        m_currentAmmo--;
    }
    return m_currentAmmo;
}

// TODO reload weapon clip?
void GrenadeLauncher::ReloadWeapon()
{
}