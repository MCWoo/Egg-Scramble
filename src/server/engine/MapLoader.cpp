#include "MapLoader.h"
#include "ObjectId.h"
#include <BulletPhysics/ConvexDecomposition/ConvexDecomposition.h>
#include <BulletPhysics/ConvexDecomposition/ConvexBuilder.h>
#include <BulletPhysics/ConvexDecomposition/cd_vector.h>
#include <BulletPhysics/ConvexDecomposition/cd_hull.h>
#include <BulletPhysics/ConvexDecomposition/cd_wavefront.h>

#include <BulletPhysics/BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletPhysics/BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletPhysics/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

MapLoader::MapLoader(btDiscreteDynamicsWorld* dynamicsWorld)
{
    m_curWorld = dynamicsWorld;
}

MapLoader::~MapLoader()
{
    // delete fileNames;
}

// Loads all the object files as Rigid Bodies into the physics world
void MapLoader::LoadMap()
{
    ConvexDecomposition::WavefrontObj wobj;

    // push all the strings for loading objects
    std::string* file;

    /*
    file = new std::string("./assets/map/new_objects/structures/barn.obj");
    fileNames.push_back((*file).c_str());
    file = new std::string("./assets/map/new_objects/structures/bench.obj");
    fileNames.push_back((*file).c_str());
    //file = new std::string("./assets/map/new_objects/structures/house.obj");
    //fileNames.push_back((*file).c_str());
    //file = new std::string("./assets/map/new_objects/structures/boat.obj");
    //fileNames.push_back((*file).c_str());
    file = new std::string("./assets/map/new_objects/structures/floating_hottub.obj");
    fileNames.push_back((*file).c_str());
    file = new std::string("./assets/map/new_objects/structures/silo.obj");
    fileNames.push_back((*file).c_str());
    file = new std::string("./assets/map/new_objects/structures/windmill.obj");
    fileNames.push_back((*file).c_str());
    file = new std::string("./assets/map/new_objects/structures/house_under_construction.obj");
    fileNames.push_back((*file).c_str());
    //file = new std::string("./assets/map/new_objects/structures/patio.obj");
    //fileNames.push_back((*file).c_str());
    //file = new std::string("./assets/map/new_objects/nature/pumpkin_patch.obj");
    //fileNames.push_back((*file).c_str());
    */

    int result;
    for (auto const& file : m_fileNames)
    {
        result = wobj.loadObj(file);
        printf("loading file: %s\n", file);

        // Error loops if the obj file wasn't loaded correctly
        while (!result)
        {
            printf("loading failed \"%s\"\n", file);
        }

        btTriangleMesh* trimesh = new btTriangleMesh();
        int i;
        for (i = 0; i < wobj.mTriCount; i++)
        {
            int index0 = wobj.mIndices[i * 3];
            int index1 = wobj.mIndices[i * 3 + 1];
            int index2 = wobj.mIndices[i * 3 + 2];

            btVector3 vertex0(wobj.mVertices[index0 * 3],
                              wobj.mVertices[index0 * 3 + 1],
                              wobj.mVertices[index0 * 3 + 2]);
            btVector3 vertex1(wobj.mVertices[index1 * 3],
                              wobj.mVertices[index1 * 3 + 1],
                              wobj.mVertices[index1 * 3 + 2]);
            btVector3 vertex2(wobj.mVertices[index2 * 3],
                              wobj.mVertices[index2 * 3 + 1],
                              wobj.mVertices[index2 * 3 + 2]);

            trimesh->addTriangle(vertex0, vertex1, vertex2);
        }

        btBvhTriangleMeshShape* tmpConvexShape = new btBvhTriangleMeshShape(trimesh, false);

        btDefaultMotionState* playerMotionState =
            new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
        // btScalar mass = 500;
        // btVector3 playerInertia(0, 0, 0);
        // convexShape->calculateLocalInertia(mass, playerInertia);
        btRigidBody::btRigidBodyConstructionInfo playerRigidBodyCI(
            0, playerMotionState, tmpConvexShape, btVector3(0, 0, 0));
        btRigidBody* pRigidBody = new btRigidBody(playerRigidBodyCI);
        pRigidBody->setFriction((btScalar)0.5);
        pRigidBody->setDamping((btScalar)100, (btScalar)100);
        pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
        m_curWorld->addRigidBody(pRigidBody);
    }

    // Windmill object                                       bottom    top

    btCollisionShape* windmill = new btCylinderShape(btVector3(30, 110, 15));
    btDefaultMotionState* playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-128, 1, 3)));
    btRigidBody::btRigidBodyConstructionInfo playerRigidBodyCIz(
        0, playerMotionState, windmill, btVector3(0, 0, 0));
    btRigidBody* pRigidBody = new btRigidBody(playerRigidBodyCIz);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    btCollisionShape* windmill_plat = new btCylinderShape(btVector3(33, 2, 33));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-129, 37, 4)));
    btRigidBody::btRigidBodyConstructionInfo platRigidBodyCIy(
        0, playerMotionState, windmill_plat, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(platRigidBodyCIy);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // patio
    btCollisionShape* patio = new btBoxShape(btVector3(32, 2, 16));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(49, 16, 111)));
    btRigidBody::btRigidBodyConstructionInfo patioCI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(patioCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(10, 15, 6));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(31.6, 0, 130)));
    btRigidBody::btRigidBodyConstructionInfo patio4CI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(patio4CI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(10, 15, 6));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(69, 0, 130)));
    btRigidBody::btRigidBodyConstructionInfo patio1CI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(patio1CI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(5, 15, 7));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(15, 0, 101)));
    btRigidBody::btRigidBodyConstructionInfo patio2CI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(patio2CI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(5, 15, 7));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(83, 0, 100)));
    btRigidBody::btRigidBodyConstructionInfo patio3CI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(patio3CI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // Rock
    btCollisionShape* rock = new btCylinderShape(btVector3(7, 11, 7));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-15.4, 0, 103)));
    btRigidBody::btRigidBodyConstructionInfo platRigidBodyCI(
        0, playerMotionState, rock, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(platRigidBodyCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // boat
    patio = new btBoxShape(btVector3(50, 2, 23));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-145, 20, 136)));
    btRigidBody::btRigidBodyConstructionInfo boatCI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(boatCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // House
    patio = new btBoxShape(btVector3(40, 50, 60));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(155, 0, 114)));
    btRigidBody::btRigidBodyConstructionInfo houseCI(
        0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(houseCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(22, 2, 60));
    playerMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(btVector3(0, 0, -1), -.60), btVector3(135, 60, 114)));
    btRigidBody::btRigidBodyConstructionInfo roof1(0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(roof1);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(22, 2, 60));
    playerMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(btVector3(0, 0, 1), -.60), btVector3(175, 60, 114)));
    btRigidBody::btRigidBodyConstructionInfo roof2(0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(roof2);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // Construction House
    patio = new btBoxShape(btVector3(36, 9, 55));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-141, 0, -121)));
    btRigidBody::btRigidBodyConstructionInfo conH(0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(conH);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    patio = new btBoxShape(btVector3(36, 1, 55));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-141, 24, -121)));
    btRigidBody::btRigidBodyConstructionInfo conH0(0, playerMotionState, patio, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(conH0);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // Silo
    btCollisionShape* silo = new btCylinderShape(btVector3(20, 63, 20));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(146.5, 0, 4)));
    btRigidBody::btRigidBodyConstructionInfo siloc(0, playerMotionState, silo, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(siloc);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // barn
    btCollisionShape* barn = new btBoxShape(btVector3(18, 50, 25));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(143.6, 0, -54)));
    btRigidBody::btRigidBodyConstructionInfo barnCI(0, playerMotionState, barn, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(barnCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    // barbara
    btCollisionShape* bb = new btBoxShape(btVector3(20, 7, 10));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-71, 0, 108)));
    btRigidBody::btRigidBodyConstructionInfo bbCI(0, playerMotionState, bb, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(bbCI);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);

    btCollisionShape* bb1 = new btBoxShape(btVector3(20, 2, 10));
    playerMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-71, 17, 108)));
    btRigidBody::btRigidBodyConstructionInfo bbCI1(0, playerMotionState, bb1, btVector3(0, 0, 0));
    pRigidBody = new btRigidBody(bbCI1);
    pRigidBody->setFriction((btScalar)0.5);
    pRigidBody->setDamping((btScalar)100, (btScalar)100);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Obstacle));
    m_curWorld->addRigidBody(pRigidBody);
}
