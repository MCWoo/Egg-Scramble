#include "Shotgun.h"
#include "SimpleBulletCollision.h"
#include "FireRateReset.h"

Shotgun::Shotgun(btDiscreteDynamicsWorld* curworld) : Weapon(curworld)
{
    m_type = WeaponType::Shotgun;
    m_nextFireTick = 0;
    m_fireRate = 30;
    m_damage = 20;
    m_currentAmmo = 100;
    m_maxAmmo = 100;
    m_gunSpeed = btVector3(0, 5, 35);
}

Shotgun::~Shotgun()
{
}

int Shotgun::UseWeapon(const btVector3& position,
                       const btMatrix3x3& rotation,
                       int playerid,
                       int teamid,
                       Entity* owner)
{
    if (m_fireFlag)
    {
        btCollisionShape* bulletShape1 = new btSphereShape(btScalar(0.5));
        btCollisionShape* bulletShape2 = new btSphereShape(btScalar(0.5));
        btCollisionShape* bulletShape3 = new btSphereShape(btScalar(0.5));
        btCollisionShape* bulletShape4 = new btSphereShape(btScalar(0.5));

        // Create bullet spray positions
        btVector3 globalPos1 = rotation * btVector3(3, 0, 3) + position;
        btVector3 globalPos2 = rotation * btVector3(2, 0, 2) + position;
        btVector3 globalPos3 = rotation * btVector3(-2, 0, 2) +position;
        btVector3 globalPos4 = rotation * btVector3(-3, 0, 3) +position;

        btDefaultMotionState* bulletMotionState1 =
            new btDefaultMotionState(btTransform(rotation, globalPos1));
        btDefaultMotionState* bulletMotionState2 =
            new btDefaultMotionState(btTransform(rotation, globalPos2));
        btDefaultMotionState* bulletMotionState3 =
            new btDefaultMotionState(btTransform(rotation, globalPos3));
        btDefaultMotionState* bulletMotionState4 =
            new btDefaultMotionState(btTransform(rotation, globalPos4));

        btScalar mass = 1;
        btVector3 bulletInertia(0, 0, 0);
        bulletShape1->calculateLocalInertia(mass, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI1(
            mass, bulletMotionState1, bulletShape1, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI2(
            mass, bulletMotionState2, bulletShape2, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI3(
            mass, bulletMotionState3, bulletShape3, bulletInertia);
        btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI4(
            mass, bulletMotionState4, bulletShape4, bulletInertia);

        btRigidBody* bRigidBody1 = new btRigidBody(bulletRigidBodyCI1);
        btRigidBody* bRigidBody2 = new btRigidBody(bulletRigidBodyCI2);
        btRigidBody* bRigidBody3 = new btRigidBody(bulletRigidBodyCI3);
        btRigidBody* bRigidBody4 = new btRigidBody(bulletRigidBodyCI4);

        btVector3 newVelocity = rotation * m_gunSpeed;

        // set bullet rotation and velocity
        bRigidBody1->setLinearVelocity(newVelocity);
        bRigidBody1->forceActivationState(DISABLE_DEACTIVATION);

        bRigidBody2->setLinearVelocity(newVelocity);
        bRigidBody2->forceActivationState(DISABLE_DEACTIVATION);

        bRigidBody3->setLinearVelocity(newVelocity);
        bRigidBody3->forceActivationState(DISABLE_DEACTIVATION);

        bRigidBody4->setLinearVelocity(newVelocity);
        bRigidBody4->forceActivationState(DISABLE_DEACTIVATION);

        // use the simple handler for the seedgun
        SimpleBulletCollision* handler1 = new SimpleBulletCollision();
        SimpleBulletCollision* handler2 = new SimpleBulletCollision();
        SimpleBulletCollision* handler3 = new SimpleBulletCollision();
        SimpleBulletCollision* handler4 = new SimpleBulletCollision();

        // Spawns bullet with this gun's damage, speed, and necessary ids into world
        Bullet* fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler1, bRigidBody1, m_curWorld);
        fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler2, bRigidBody2, m_curWorld);
        fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler3, bRigidBody3, m_curWorld);
        fireProjectile = EntitySpawner::Instance()->SpawnBullet(
            playerid, teamid, m_damage, m_type, handler4, bRigidBody4, m_curWorld);

        m_fireFlag = 0;
        m_nextFireTick = FireRateReset::Instance()->m_currentWorldTick + m_fireRate;

        // add used weapon to "used" list in FireRateReset static object
        FireRateReset::Instance()->AddWeapon(this);
        m_currentAmmo--;
    }
    return m_currentAmmo;
}