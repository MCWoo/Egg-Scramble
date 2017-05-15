#pragma once

#include "Basic/Singleton.h"
#include "network/GameData.h"

#include <map>

class Bullet;
class BulletCollisionHandler;
class btDiscreteDynamicsWorld;
class btRigidBody;
class Collectable;
class Entity;
class Flag;
class Player;

class EntitySpawner : public Singleton<EntitySpawner>
{
    friend class Singleton<EntitySpawner>;

private:
    std::map<std::pair<ClassId, unsigned int>, Entity*> m_entities;

    EntitySpawner() = default;
    ~EntitySpawner() = default;

    unsigned int m_oidPlayer = 0;      // player
    unsigned int m_oidFlag = 0;        // flag
    unsigned int m_oidBullet = 0;      // bullet
    unsigned int m_oidCollectable = 0; // collectable

public:
    Player* SpawnPlayer(int teamid, PosInfo pos, btDiscreteDynamicsWorld* physicsWorld);

    Flag* SpawnFlag(PosInfo pos, btDiscreteDynamicsWorld* physicsWorld);

    // id of player spawning, team id of player spawning, dmg the bullet does, the type of weapon
    // that shot the bullet, the collision handler for the bullet, the bullet physics body, the
    // world
    Bullet* SpawnBullet(int playerid,
                        int teamid,
                        int damage,
                        WeaponType shooter,
                        BulletCollisionHandler* handler,
                        btRigidBody* bullet_body,
                        btDiscreteDynamicsWorld* physicsWorld);

    void SpawnCollectable(btDiscreteDynamicsWorld* curworld, WeaponType w_type);

    void SpawnCollectable(btDiscreteDynamicsWorld* curworld, PowerUpType p_type);

    void AddEntity(ClassId cid, unsigned int oid, Entity* ent);
    Entity* GetEntity(ClassId cid, unsigned int oid);
    void RemoveEntity(ClassId cid, unsigned int oid);

    std::map<std::pair<ClassId, unsigned int>, Entity*>& GetMap();

    static std::pair<int, int> GetRandomLoc();
};