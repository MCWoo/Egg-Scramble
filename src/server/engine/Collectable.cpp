#include "Collectable.h"

#include "CollectableSpawner.h"
#include "EntitySpawner.h"
#include "Player.h"
#include "PowerUp.h"
#include "Weapon.h"

#include <ctime>

Collectable::Collectable(int objectid, PosInfo pos, btDiscreteDynamicsWorld* curworld, Weapon* wp)
    : Entity(ClassId::Collectable, objectid, curworld)
{
    m_weapon = wp;
    btCollisionShape* collectableShape = new btBoxShape(btVector3(1, 1, 1));

    // Create Collectable physics object
    btDefaultMotionState* collectableMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
    btScalar mass = 1;
    btVector3 collectableInertia(0, 0, 0);
    collectableShape->calculateLocalInertia(mass, collectableInertia);
    btRigidBody::btRigidBodyConstructionInfo collectableRigidBodyCI(
        mass, collectableMotionState, collectableShape, collectableInertia);
    btRigidBody* pRigidBody = new btRigidBody(collectableRigidBodyCI);
    m_curWorld->addRigidBody(pRigidBody);

    // Set Collectable's protected fields
    m_entityRigidBody = pRigidBody;
    m_type = CollectType::Weapon;

    // Set RigidBody to point to Collectable
    pRigidBody->setUserPointer(this);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Collectable));
}

Collectable::Collectable(int objectid,
                         PosInfo pos,
                         btDiscreteDynamicsWorld* curworld,
                         PowerUp* powerup)
    : Entity(ClassId::Collectable, objectid, curworld)
{
    m_powerup = powerup;
    btCollisionShape* collectableShape = new btBoxShape(btVector3(1, 1, 1));

    // Create Collectable physics object
    btDefaultMotionState* collectableMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
    btScalar mass = 1;
    btVector3 collectableInertia(0, 0, 0);
    collectableShape->calculateLocalInertia(mass, collectableInertia);
    btRigidBody::btRigidBodyConstructionInfo collectableRigidBodyCI(
        mass, collectableMotionState, collectableShape, collectableInertia);
    btRigidBody* pRigidBody = new btRigidBody(collectableRigidBodyCI);
    m_curWorld->addRigidBody(pRigidBody);

    // Set Collectable's protected fields
    m_entityRigidBody = pRigidBody;
    m_type = CollectType::PowerUp;

    // Set RigidBody to point to Collectable
    pRigidBody->setUserPointer(this);
    pRigidBody->setUserIndex(static_cast<int>(ClassId::Collectable));
}

// NOTE***  Version if Weapon is only thing collectable
void Collectable::HandleCollect(Player* collidedPlayer)
{
    // Remove collectable object from EntitySpawner Map
    EntitySpawner::Instance()->RemoveEntity(ClassId::Collectable, m_objectId);
    CollectableSpawner::Instance()->DecCollectables();

    if (m_type == CollectType::Weapon)
    {
        // If player already has usable
        if (collidedPlayer->HasWeapon())
        {
            if (m_weapon->GetWeaponType() == collidedPlayer->GetPlayerWeaponType())
            {
                collidedPlayer->GetWeapon()->ReloadWeapon();
            }

            return;
        }

        printf("acquired weapon of m_type %d\n", m_weapon->GetWeaponType());
        collidedPlayer->EquipWeapon(m_weapon);
    }
    else if (m_type == CollectType::PowerUp)
    {
        // always apply health gain
        if (m_powerup->GetType() == PowerUpType::HealthGain)
        {
            m_powerup->ApplyPower(collidedPlayer);
            return;
        }

        if (collidedPlayer->HasPower())
        {
            if (m_powerup->GetType() == collidedPlayer->GetPower()->GetType())
            {
                m_powerup->ApplyPower(collidedPlayer);
            }
            return;
        }

        m_powerup->ApplyPower(collidedPlayer);

        printf("acquired powerup of type %d\n", m_powerup->GetType());
    }
}

Collectable::~Collectable()
{
    if (m_entityRigidBody)
    {
        m_curWorld->removeCollisionObject(m_entityRigidBody);
        delete m_entityRigidBody->getMotionState();
        delete m_entityRigidBody->getCollisionShape();
        delete m_entityRigidBody;
    }
}