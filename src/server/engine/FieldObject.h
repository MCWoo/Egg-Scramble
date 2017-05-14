#pragma once

#include <BulletPhysics/BulletCollision/CollisionDispatch/btGhostObject.h>

#ifndef BULLET_PHYSICS
#define BULLET_PHYSICS
#include <BulletPhysics/btBulletDynamicsCommon.h>
#include <BulletPhysics/btBulletCollisionCommon.h>
#endif

class Entity;

class FieldObject
{
protected:
    btPairCachingGhostObject* m_fieldGhostObject = nullptr;
    btDiscreteDynamicsWorld* m_curWorld = nullptr;
    int m_teamId = 0;

public:
    FieldObject(const btVector3& origin,
                btCollisionShape* fieldshape,
                int team_id,
                btDiscreteDynamicsWorld* curworld);
    virtual ~FieldObject();

    btPairCachingGhostObject* GetFieldGhostObject() { return m_fieldGhostObject; }
    // handles field detection. Returns 1 when field is done, 0 if not
    virtual int HandleField();
};
