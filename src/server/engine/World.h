#pragma once

#include "EntitySpawner.h"
#include "network/GameData.h"

#include <vector>
#include <memory>
#include <map>

class WorldObstacle;
class MapLoader;

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;

typedef std::vector<PosInfo> pos_list;

class World
{
private:
    // list of game world objects
    std::vector<std::shared_ptr<Flag>> m_flags;
    // Delete list, mainly for bullets
    std::vector<Entity*> m_deleteList;
    // Marked list to protect multi-handling, for flags
    std::vector<Entity*> m_markedList;
    // List of entities to unmark at the end
    std::vector<Entity*> m_unmarkList;

    // Physics World attributes
    btDiscreteDynamicsWorld* m_curWorld = nullptr;
    btDefaultCollisionConfiguration* m_colConfig = nullptr;
    btCollisionDispatcher* m_disp = nullptr;
    btBroadphaseInterface* m_pairCache = nullptr;
    btSequentialImpulseConstraintSolver* m_solv = nullptr;

    // Map objects
    WorldObstacle* m_ground = nullptr;
    WorldObstacle* m_frontWall = nullptr;
    WorldObstacle* m_backWall = nullptr;
    WorldObstacle* m_leftWall = nullptr;
    WorldObstacle* m_rightWall = nullptr;

    // object ids
    int m_oid = -1;

    // Map Loader object
    MapLoader* m_worldMapLoader = nullptr;

    // list of fields to check.  Explosions and mounts.  Maybe decouple into a class of it's own.
    // Have a TTL

public:
    World();
    ~World();

    // ticker used for now
    unsigned int m_worldTick = 0;

    void Init();

    void PreSpawn();
    btDiscreteDynamicsWorld* GetPhysicsWorld();

    // Updates Physics world by one tick
    void Update();

    // Finds and Removes flag from world list of flags
    void RemoveFlag(Flag* collectedFlag);
};