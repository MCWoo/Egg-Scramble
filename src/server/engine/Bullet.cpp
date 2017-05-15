#include "Bullet.h"

#include "EntitySpawner.h"
#include "BulletCollisionHandler.h"

Bullet::Bullet(unsigned int objectid,
               int playerid,
               int teamid,
               int damage,
               BulletCollisionHandler* handler,
               btRigidBody* bullet_body,
               btDiscreteDynamicsWorld* physicsWorld)
    : Entity(ClassId::Bullet, objectid, physicsWorld)
    , m_collisionHandler(handler)
    , m_playerId(playerid)
    , m_teamId(teamid)
    , m_damage(damage)
{
    physicsWorld->addRigidBody(bullet_body);

    m_entityRigidBody = bullet_body;

    bullet_body->setUserPointer(this);
    bullet_body->setUserIndex(static_cast<int>(ClassId::Bullet));
}

Bullet::~Bullet()
{
    m_curWorld->removeCollisionObject(m_entityRigidBody);
    delete m_entityRigidBody->getMotionState();
    delete m_entityRigidBody->getCollisionShape();
    delete m_entityRigidBody;
    delete m_collisionHandler;
    EntitySpawner::Instance()->RemoveEntity(ClassId::Bullet, m_objectId);
}

btVector3 Bullet::GetBulletPosition()
{
    return (m_entityRigidBody)->getCenterOfMassPosition();
}

int Bullet::GetPlayerId()
{
    return m_playerId;
}

int Bullet::GetTeamId()
{
    return m_teamId;
}

int Bullet::GetDamage()
{
    return m_damage;
}
