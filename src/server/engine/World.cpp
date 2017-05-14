#include "World.h"
#include "ObjectId.h"
#include "../ServerGame.h"
#include "FireRateReset.h"
#include "FieldHandler.h"
#include "RespawnHandler.h"
#include "CollectableSpawner.h"
#include "Player.h"
#include "Flag.h"
#include "Bullet.h"
#include "WorldObstacle.h"
#include <BulletPhysics/BulletCollision/CollisionDispatch/btGhostObject.h>
#include "MapLoader.h"

World::World()
{
    // initialize map objects
}

World::~World()
{
    // TODO handle vector deletes...
    delete m_curWorld;
}

void World::Init()
{
    int z = 1000; // this is a random number for the walls right now, we need to change this

    // Create Physics world
    btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    btDiscreteDynamicsWorld* dynamicsWorld =
        new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
    dynamicsWorld->setGravity(btVector3(0, -20, 0));

    // Add Ground Object
    // btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(btScalar(0.), btScalar(1.),
    // btScalar(0.)), 0);
    btCollisionShape* groundShape =
        new btBoxShape(btVector3(Constants::worldWidth + 50, 1, Constants::worldWidth + 50));
    btDefaultMotionState* groundMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
        0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_friction = 1.0;
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(groundRigidBody);
    groundRigidBody->setGravity(btVector3(0, 0, 0));
    groundRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create Ground Obstacle
    WorldObstacle* groundwall = new WorldObstacle(z++, groundRigidBody, m_curWorld);

    // Add Sky Object
    btCollisionShape* skyShape =
        new btBoxShape(btVector3(Constants::worldWidth + 50, 1, Constants::worldWidth + 50));
    btDefaultMotionState* skyMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, Constants::worldHeight, 0)));
    btRigidBody::btRigidBodyConstructionInfo skyRigidBodyCI(
        0, skyMotionState, skyShape, btVector3(0, 0, 0));
    skyRigidBodyCI.m_friction = .4;
    btRigidBody* skyRigidBody = new btRigidBody(skyRigidBodyCI);
    skyRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(skyRigidBody);
    skyRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create Ground Obstacle
    WorldObstacle* skywall = new WorldObstacle(z++, skyRigidBody, m_curWorld);

    // Add Pos X Wall
    // btCollisionShape* xWallShape = new btStaticPlaneShape(btVector3(btScalar(-1.), btScalar(0.),
    // btScalar(0.)), 0);
    btCollisionShape* xWallShape =
        new btBoxShape(btVector3(1, Constants::worldHeight, Constants::worldWidth));
    btDefaultMotionState* xWallMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(Constants::worldWidth + 1, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo xWallRigidBodyCI(
        0, xWallMotionState, xWallShape, btVector3(0, 0, 0));
    xWallRigidBodyCI.m_friction = .5;
    btRigidBody* xWallRigidBody = new btRigidBody(xWallRigidBodyCI);
    xWallRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(xWallRigidBody);
    xWallRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create X Wall
    WorldObstacle* xwall = new WorldObstacle(z++, xWallRigidBody, m_curWorld);

    // Add Neg X Wall
    // btCollisionShape* nxWallShape = new btStaticPlaneShape(btVector3(btScalar(1.), btScalar(0.),
    // btScalar(0.)), 0);
    btCollisionShape* nxWallShape =
        new btBoxShape(btVector3(1, Constants::worldHeight, Constants::worldWidth));
    btDefaultMotionState* nxWallMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(-(Constants::worldWidth + 1), 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo nxWallRigidBodyCI(
        0, nxWallMotionState, nxWallShape, btVector3(0, 0, 0));
    nxWallRigidBodyCI.m_friction = .5;
    btRigidBody* nxWallRigidBody = new btRigidBody(nxWallRigidBodyCI);
    nxWallRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(nxWallRigidBody);
    nxWallRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create Neg X Wall
    WorldObstacle* nxwall = new WorldObstacle(z++, groundRigidBody, m_curWorld);

    // Add Pos Z Wall
    // btCollisionShape* zWallShape = new btStaticPlaneShape(btVector3(btScalar(0.), btScalar(0.),
    // btScalar(-1.)), 0);
    btCollisionShape* zWallShape =
        new btBoxShape(btVector3(Constants::worldWidth, Constants::worldHeight, 1));
    btDefaultMotionState* zWallMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, Constants::worldWidth + 1)));
    btRigidBody::btRigidBodyConstructionInfo zWallRigidBodyCI(
        0, zWallMotionState, zWallShape, btVector3(0, 0, 0));
    zWallRigidBodyCI.m_friction = .5;
    btRigidBody* zWallRigidBody = new btRigidBody(zWallRigidBodyCI);
    zWallRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(zWallRigidBody);
    zWallRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create Pos Z Wall
    WorldObstacle* zwall = new WorldObstacle(z++, zWallRigidBody, m_curWorld);

    // Add Neg Z Wall
    // btCollisionShape* nzWallShape = new btStaticPlaneShape(btVector3(btScalar(0.), btScalar(0.),
    // btScalar(1.)), 0);
    btCollisionShape* nzWallShape =
        new btBoxShape(btVector3(Constants::worldWidth, Constants::worldHeight, 1));
    btDefaultMotionState* nzWallMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -(Constants::worldWidth + 1))));
    btRigidBody::btRigidBodyConstructionInfo nzWallRigidBodyCI(
        0, nzWallMotionState, zWallShape, btVector3(0, 0, 0));
    nzWallRigidBodyCI.m_friction = .5;
    btRigidBody* nzWallRigidBody = new btRigidBody(nzWallRigidBodyCI);
    nzWallRigidBody->setGravity(btVector3(0, 0, 0));
    dynamicsWorld->addRigidBody(nzWallRigidBody);
    nzWallRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    // Create Neg Z Wall
    WorldObstacle* nzwall = new WorldObstacle(z++, nzWallRigidBody, m_curWorld);

    // Load in world objects
    m_worldMapLoader = new MapLoader(dynamicsWorld);
    m_worldMapLoader->LoadMap();

    // Set Local attributes
    m_ground = groundwall;
    m_frontWall = nzwall;
    m_backWall = zwall;
    m_leftWall = nxwall;
    m_rightWall = xwall;

    m_curWorld = dynamicsWorld;
    m_solv = solver;
    m_pairCache = overlappingPairCache;
    m_disp = dispatcher;
    m_colConfig = collisionConfig;

    // set up physics world for field detection
    dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(
        new btGhostPairCallback());
}

btDiscreteDynamicsWorld* World::GetPhysicsWorld()
{
    return m_curWorld;
}

void World::PreSpawn()
{
    ServerGame::Instance()->SendReadyToSpawnPacket();
}

void World::Update()
{
    // Step simulation
    m_curWorld->stepSimulation(1 / 60.f, 4); // 60
    //    m_curWorld->stepSimulation(1 / 30.f, 4);   // 30 frames

    FireRateReset::Instance()->m_currentWorldTick++;

    // Process Weapon Fire Rate Reset and field collisions
    FireRateReset::Instance()->ResetWeapons();
    FieldHandler::Instance()->HandleFields();

    // Process all collisions
    int numManifolds = m_curWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        // printf("numMan  %d\n", numManifolds);
        btPersistentManifold* contactManifold =
            m_curWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        // Handle Bullet Collisions--------------------------
        // If object A of collision is a Bullet
        if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Bullet)
        {
            // Grab Bullet Object
            Bullet* collideBullet = (Bullet*)obA->getUserPointer();
            if (collideBullet->GetMarked())
            {
                continue;
            }
            // If it hit a player
            if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Player)
            {
                Player* collidePlayer = (Player*)obB->getUserPointer();
                // ignore friendly fire
                if (collideBullet->GetTeamId() == collidePlayer->GetTeamId())
                {
                    continue;
                }
                // printf("Pushed to delete!, hit playerB");
                collideBullet->SetToMarked(m_worldTick);
                if (collideBullet->HandleBulletCollision(m_worldTick, collidePlayer))
                {
                    m_deleteList.push_back(collideBullet);
                    ServerGame::Instance()->SendRemovePacket(ClassId::Bullet,
                                                             collideBullet->GetObjectId());
                }
                else
                {
                    m_unmarkList.push_back(collideBullet);
                }
                // TODO send "you got hit"
                if (collidePlayer->takeDamage(collideBullet->GetDamage(), m_worldTick))
                {
                    // printf("Player is dead!");
                    // TODO Handle Player death:  send player death to client
                }
            }

            // If it hit a bullet, ignore and just bounce off
            else if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Bullet
                     || static_cast<ClassId>(obB->getUserIndex()) == ClassId::Field)
            {
                continue;
            }
            else
            { // Collided with unknown object
                // deletes bulletA regardless
                // printf("Pushed to delete!, hit ground B,  %d", obB->getUserIndex());
                btVector3 bulPos = collideBullet->GetEntityPosition();
                // printf("Current position:  x: %f, y: %f, z: %f  \n", bulPos.getX(),
                // bulPos.getY(), bulPos.getZ());
                bulPos = collideBullet->GetRigidBody()->getLinearVelocity();
                // printf("Current velocity:  x: %f, y: %f, z: %f  \n", bulPos.getX(),
                // bulPos.getY(), bulPos.getZ());
                if (collideBullet->GetMarked())
                {
                    continue;
                }
                collideBullet->SetToMarked(m_worldTick);
                if (collideBullet->HandleBulletCollision(m_worldTick, nullptr))
                {
                    m_deleteList.push_back(collideBullet);
                    ServerGame::Instance()->SendRemovePacket(ClassId::Bullet,
                                                             collideBullet->GetObjectId());
                }
                else
                {
                    m_unmarkList.push_back(collideBullet);
                }
            }
        }

        // If object B of collision is a Bullet
        else if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Bullet)
        {
            // Grab Bullet Object
            Bullet* collideBullet = (Bullet*)obB->getUserPointer();
            if (collideBullet->GetMarked())
            {
                continue;
            }
            // Bullet hits player
            if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Player)
            {
                Player* collidePlayer = (Player*)obA->getUserPointer();
                // ignore team fire
                if (collideBullet->GetTeamId() == collidePlayer->GetTeamId())
                {
                    continue;
                }

                collideBullet->SetToMarked(m_worldTick);
                if (collideBullet->HandleBulletCollision(m_worldTick, collidePlayer))
                {
                    m_deleteList.push_back(collideBullet);
                    ServerGame::Instance()->SendRemovePacket(ClassId::Bullet,
                                                             collideBullet->GetObjectId());
                }
                else
                {
                    m_unmarkList.push_back(collideBullet);
                }
                if (collidePlayer->takeDamage(collideBullet->GetDamage(), m_worldTick))
                {
                    // printf("Player is dead!");
                    // TODO Handle Player death:  send player death to client
                }
            }

            // If it hit a bullet, ignore and bounce off
            else if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Bullet
                     || static_cast<ClassId>(obB->getUserIndex()) == ClassId::Field)
            {
                continue;
            }
            else // Collide with unknown object
            {
                if (collideBullet->GetMarked())
                {
                    continue;
                }
                // deletes bulletB regardless
                // printf("Pushed to delete!, hit ground A,  %d", obA->getUserIndex());
                btVector3 bulPos = collideBullet->GetEntityPosition();
                // printf("Current position:  x: %f, y: %f, z: %f  \n", bulPos.getX(),
                // bulPos.getY(), bulPos.getZ());
                bulPos = collideBullet->GetRigidBody()->getLinearVelocity();
                // printf("Current velocity:  x: %f, y: %f, z: %f  \n", bulPos.getX(),
                // bulPos.getY(), bulPos.getZ());
                collideBullet->SetToMarked(m_worldTick);
                if (collideBullet->HandleBulletCollision(m_worldTick, nullptr))
                {
                    m_deleteList.push_back(collideBullet);
                    ServerGame::Instance()->SendRemovePacket(ClassId::Bullet,
                                                             collideBullet->GetObjectId());
                }
                else
                {
                    m_unmarkList.push_back(collideBullet);
                }
            }
        }

        // Handle Player Collisions -------------------------------------------------
        // Obj A is Player
        if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Player)
        {
            // Grab Player Object
            Player* collidePlayer = (Player*)obA->getUserPointer();

            // TODO, if Obj B is collectable, HandleCollectable();
            if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Collectable)
            {
                // Ignore dead guy
                if (!collidePlayer->IsAlive())
                    continue;

                // Handle Collectable Collection
                Collectable* collectObj = (Collectable*)obB->getUserPointer();
                // check if collectable has been handled already
                if (collectObj->GetMarked())
                {
                    continue;
                }
                collectObj->HandleCollect(collidePlayer);
                if (collectObj->GetType() == CollectType::Weapon)
                {
                    ServerGame::Instance()->SendRemovePacket(
                        ClassId::Collectable,
                        collectObj->GetObjectId(),
                        ClassId::Player,
                        collidePlayer->GetObjectId(),
                        CollectType::Weapon,
                        static_cast<int>(collectObj->GetWeapon()->GetWeaponType()));
                }
                else if (collectObj->GetType() == CollectType::PowerUp)
                {
                    ServerGame::Instance()->SendRemovePacket(
                        ClassId::Collectable,
                        collectObj->GetObjectId(),
                        ClassId::Player,
                        collidePlayer->GetObjectId(),
                        CollectType::PowerUp,
                        static_cast<int>(collectObj->GetPowerUp()->GetType()));
                }
                m_deleteList.push_back(collectObj);
                collectObj->SetToMarked(m_worldTick);
            }

            // if Obj B is Flag
            else if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Flag)
            {
                // Ignore dead guy
                if (!collidePlayer->IsAlive())
                    continue;

                // Handle Flag Collection
                Flag* collideFlag = (Flag*)obB->getUserPointer();
                if (collideFlag->GetMarked())
                {
                    continue;
                }

                collideFlag->HandleCollectable(collidePlayer);
                ServerGame::Instance()->SendRemovePacket(ClassId::Flag,
                                                         collideFlag->GetObjectId(),
                                                         ClassId::Player,
                                                         collidePlayer->GetObjectId());
                m_markedList.push_back(collideFlag);
                collideFlag->SetToMarked(m_worldTick);
                // TODO send a packet for the player to acquire the item for GUI
            }

            // Handles Jump Semaphore
            else // if (++y % 50000 == 0)
            {
                int numContacts = contactManifold->getNumContacts();
                for (int j = 0; j < numContacts; j++)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance() < 0.f)
                    {
                        const btVector3& ptA = pt.getPositionWorldOnA();
                        const btVector3& ptB = pt.getPositionWorldOnB();
                        const btVector3& normalOnB = pt.m_normalWorldOnB;

                        // Reset Jump Semaphore, detects collision off of non-player, error of .1
                        // TODO: FIX MAGIC NUMBER OF ClassId::Player HALFEXTENT = 1
                        if ((collidePlayer->GetEntityPosition()).getY() - 1 > (ptB.getY() - .1)
                            && (collidePlayer->GetEntityPosition()).getY() - 1 < (ptB.getY() + .1))
                        {
                            collidePlayer->ResetJump();
                        }

                        /*
                        Notes: No Rotation: Can minus halfextent of player object from pt of
                        collision ~= 0
                        Notes: Rotation:  Collision pt - (halfextent * normalOnB) ~= 0
                        printf(" ptA at (%f,%f,%f)\n", ptA.getX(), ptA.getY(), ptA.getZ());
                        printf(" ptB at (%f,%f,%f)\n", ptB.getX(), ptB.getY(), ptB.getZ());
                        printf(" normalOnB at (%f,%f,%f)\n", normalOnB.getX(), normalOnB.getY(),
                        normalOnB.getZ());
                        */
                    }
                }
            }
        }

        // Obj B is Player
        else if (static_cast<ClassId>(obB->getUserIndex()) == ClassId::Player)
        {
            // Grab Player Object
            Player* collidePlayer = (Player*)obB->getUserPointer();

            // If Obj A is collectable, HandleCollectable();
            if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Collectable)
            {
                // Ignore dead guy
                if (!collidePlayer->IsAlive())
                    continue;

                // Handle Collectable Collection
                Collectable* collectObj = (Collectable*)obA->getUserPointer();
                // check if collectable has been handled already
                if (collectObj->GetMarked())
                {
                    continue;
                }
                collectObj->HandleCollect(collidePlayer);
                if (collectObj->GetType() == CollectType::Weapon)
                {
                    ServerGame::Instance()->SendRemovePacket(
                        ClassId::Collectable,
                        collectObj->GetObjectId(),
                        ClassId::Player,
                        collidePlayer->GetObjectId(),
                        CollectType::Weapon,
                        static_cast<int>(collectObj->GetWeapon()->GetWeaponType()));
                }
                else if (collectObj->GetType() == CollectType::PowerUp)
                {
                    ServerGame::Instance()->SendRemovePacket(
                        ClassId::Collectable,
                        collectObj->GetObjectId(),
                        ClassId::Player,
                        collidePlayer->GetObjectId(),
                        CollectType::PowerUp,
                        static_cast<int>(collectObj->GetPowerUp()->GetType()));
                }
                m_deleteList.push_back(collectObj);
                collectObj->SetToMarked(m_worldTick);
            }

            // if Obj A is Flag
            else if (static_cast<ClassId>(obA->getUserIndex()) == ClassId::Flag)
            {
                // Ignore dead guy
                if (!collidePlayer->IsAlive())
                    continue;

                // Handle Flag Collection
                Flag* collideFlag = (Flag*)obA->getUserPointer();
                if (collideFlag->GetMarked())
                {
                    continue;
                }
                collideFlag->HandleCollectable(collidePlayer);
                ServerGame::Instance()->SendRemovePacket(ClassId::Flag,
                                                         collideFlag->GetObjectId(),
                                                         ClassId::Player,
                                                         collidePlayer->GetObjectId());
                m_markedList.push_back(collideFlag);
                collideFlag->SetToMarked(m_worldTick);
                // TODO send a packet for the player to acquire the item for GUI
            }

            // Handles Jump Semaphore
            else // if (++y % 50000 == 0)
            {
                int numContacts = contactManifold->getNumContacts();
                for (int j = 0; j < numContacts; j++)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance() < 0.f)
                    {
                        const btVector3& ptA = pt.getPositionWorldOnA();
                        const btVector3& ptB = pt.getPositionWorldOnB();
                        const btVector3& normalOnB = pt.m_normalWorldOnB;

                        // Reset Jump Semaphore, detects collision off of non-player, error of .1
                        // TODO: FIX MAGIC NUMBER OF ClassId::Player HALFEXTENT = 1
                        if ((collidePlayer->GetEntityPosition()).getY() - 1 > (ptA.getY() - .1)
                            && (collidePlayer->GetEntityPosition()).getY() - 1 < (ptA.getY() + .1))
                        {
                            collidePlayer->ResetJump();
                        }

                        /*
                        Notes: No Rotation: Can minus halfextent of player object from pt of
                        collision ~= 0
                        Notes: Rotation:  Collision pt - (halfextent * normalOnB) ~= 0
                        printf(" ptA at (%f,%f,%f)\n", ptA.getX(), ptA.getY(), ptA.getZ());
                        printf(" ptB at (%f,%f,%f)\n", ptB.getX(), ptB.getY(), ptB.getZ());
                        printf(" normalOnB at (%f,%f,%f)\n", normalOnB.getX(), normalOnB.getY(),
                        normalOnB.getZ());
                        */
                    }
                }
            }
        }
    }

    // Reset Marked Entities
    for (auto it = m_markedList.begin(); it != m_markedList.end(); it++)
    {
        (*it)->ResetMark();
        m_curWorld->removeCollisionObject((*it)->GetRigidBody());
    }
    m_markedList.clear();

    // Delete Marked Entities
    for (auto it = m_deleteList.begin(); it != m_deleteList.end(); it++)
    {
        delete (*it);
    }
    m_deleteList.clear();

    // Unmarks entities not on the previous lists, used for bullet collision
    for (auto it = m_unmarkList.begin(); it != m_unmarkList.end(); it++)
    {
        if ((*it)->GetMarkTick() == m_worldTick - 5)
            (*it)->ResetMark();
    }

    // if (x++ % 10000 == 0) {
    if (m_worldTick % 500 == 0)
    {
        /*
        btVector3 vecg = rightWall->getCenterOfMassPosition();
        printf(" right wall at (%f,%f,%f)\n", vecg.getX(), vecg.getY(), vecg.getZ());
        vecg = leftWall->getCenterOfMassPosition();
        printf(" left wall at (%f,%f,%f)\n", vecg.getX(), vecg.getY(), vecg.getZ());
        vecg = frontWall->getCenterOfMassPosition();
        printf(" front wall at (%f,%f,%f)\n", vecg.getX(), vecg.getY(), vecg.getZ());
        vecg = backWall->getCenterOfMassPosition();
        printf(" back wall at (%f,%f,%f)\n", vecg.getX(), vecg.getY(), vecg.getZ());
        */

        auto& dynamicMap = EntitySpawner::Instance()->GetMap();
        for (auto it = dynamicMap.begin(); it != dynamicMap.end(); it++)
        {
            btVector3 vec = it->second->GetEntityPosition();
            // printf(" Dynamic object classid: %d, objid: %d, at (%f,%f,%f)\n",
            // it->second->GetClassId(), it->second->GetObjectId(), vec.getX(), vec.getY(),
            // vec.getZ());
        }

        /*
        for (std::vector<std::shared_ptr<Flag> >::iterator it = flags.begin(); it != flags.end();
        ++it)
        {
            btVector3 vec = (*it)->GetEntityPosition();
            printf(" flag at (%f,%f,%f)\n", vec.getX(), vec.getY(), vec.getZ());
        }
        */
    }

    // Handle spawning for this tick
    RespawnHandler::Instance()->RespawnPlayers(m_worldTick);
    CollectableSpawner::Instance()->SpawnRandomCollectables(m_curWorld, m_worldTick);

    // Send position updates of all dynamic objects
    if (m_worldTick % 4 == 0)
    {
        // Iterates through all dynamic objects in the Map and sends position updates to client
        auto& dynamicMap = EntitySpawner::Instance()->GetMap();
        for (auto it = dynamicMap.begin(); it != dynamicMap.end(); it++)
        {
            // btVector3 vec = it->second->GetEntityPosition();
            btVector3 vec = it->second->GetRigidBody()->getLinearVelocity();
            float thresh = .0005;
            // don't send packets if the object is stationary?
            if ((abs(vec.getX()) < thresh && abs(vec.getY()) < thresh && abs(vec.getZ()) < thresh))
            {
                if (static_cast<ClassId>(it->second->GetClassId()) == ClassId::Player)
                {
                    ServerGame::Instance()->SendMovePacket((ClassId)it->second->GetClassId(),
                                                           it->second->GetObjectId());
                }
                // printf(" Dynamic object classid: %d, objid: %d, velocity (%f,%f,%f)\n",
                // it->second->GetClassId(), it->second->GetObjectId(), vec.getX(), vec.getY(),
                // vec.getZ());
                continue;
            }

            // modify on player ticks
            if (static_cast<ClassId>(it->second->GetClassId()) == ClassId::Player)
            {
                Player* player = static_cast<Player*>(it->second);
                // resets stuns
                if (player->GetStun() > 0)
                {
                    player->SetStun(player->GetStun() - 4); // unstun the guy
                }

                // resets powerups
                if (player->GetPowerUpDuration() > 0)
                {
                    player->SetPowerUpDuration(player->GetPowerUpDuration() - 4);
                    if (player->GetPowerUpDuration() <= 0)
                    {
                        if (!player->GetPower())
                        {
                            player->GetPower()->RemovePower(player);
                        }
                    }
                }
            }

            ServerGame::Instance()->SendMovePacket((ClassId)it->second->GetClassId(),
                                                   it->second->GetObjectId());
        }
    }

    m_worldTick++;
}

void World::RemoveFlag(Flag* collectedFlag)
{
    for (auto it = m_flags.begin(); it != m_flags.end(); ++it)
    {
        if (collectedFlag == it->get())
        {
            m_flags.erase(it);
            printf("Flag has been removed from world list \n");
            return;
        }
    }
}
