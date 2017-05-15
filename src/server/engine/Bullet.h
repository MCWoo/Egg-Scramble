#pragma once

#include "Entity.h"
#include "BulletCollisionHandler.h"

class BulletCollisionHandler;

class Bullet : public Entity
{
private:
    int m_playerId = 0;
    int m_teamId = 0;
    int m_damage = 0;
    BulletCollisionHandler* m_collisionHandler = nullptr;

public:
    Bullet(unsigned int objectid,
           int playerid,
           int teamid,
           int damage,
           BulletCollisionHandler* handler,
           btRigidBody* bullet_body,
           btDiscreteDynamicsWorld* physicsWorld);

    virtual ~Bullet();

    btVector3 GetBulletPosition();

    // the tick that the collision occured, may consider the object it collided with, returns true
    // if the bullet needs to be deleted, the handler decides this
    // collidee will be nullptr if the collidee is a static object
    bool HandleBulletCollision(unsigned int world_tick, Entity* collidee)
    {
        return m_collisionHandler->HandleBulletCollision(world_tick, collidee);
    }

    // get player id
    int GetPlayerId();

    // get team id
    int GetTeamId();

    int GetDamage();
};