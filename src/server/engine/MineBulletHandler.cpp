#include "MineBulletHandler.h"

#include "BlastField.h"
#include "FieldHandler.h"
#include "Player.h"

MineBulletHandler::MineBulletHandler(FieldObject* field) : m_field(field)
{
}

bool MineBulletHandler::HandleBulletCollision(unsigned int world_tick, Entity* collidee)
{
    // don't delete if static object
    if (!collidee || !m_bullet)
        return false;

    if (collidee->GetClassId() == ClassId::Player)
    {
        // btVector3 temp = m_bullet->GetEntityPosition();
        // btVector3* position = new btVector3(temp.getX(), temp.getY(), temp.getZ());

        // btCollisionShape* expSphere = new btSphereShape(btScalar(explosion_size));
        // BlastField* explosionField = new BlastField(nonFriendly, magnitude, ttl,
        // m_bullet->GetDamage(), position, expSphere, m_bullet->GetTeamId(),
        // m_bullet->GetPhysicsWorld());
        // setting location of the field
        Player* p = static_cast<Player*>(collidee);
        if (p->GetTeamId() != m_bullet->GetTeamId())
        {
            btVector3 temp = m_bullet->GetEntityPosition();
            btTransform currentTrans = m_field->GetFieldGhostObject()->getWorldTransform();
            currentTrans.setOrigin(temp);
            m_field->GetFieldGhostObject()->setWorldTransform(currentTrans);
            FieldHandler::Instance()->AddField(m_field);
            return true;
        }
    }
    return false;
}