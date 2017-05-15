#pragma once
#include "FieldObject.h"
#include "ObjectId.h"

#include <map>

// a blast field that does damage and knocks back
class BlastField : public FieldObject
{
protected:
    int m_fieldTtl = 0;
    int m_fieldDamage = 0;
    int m_magnitude = 0; // magnitude in which the blast will knock back
    int m_stun = 0;
    bool m_nonFriendly = false; // Whether or not this field friendly fires, true if it does

    static int const s_yDirection = 20; // the y direction magnitude to launch the player

    // list of entities that already got blasted by the velocity
    std::map<std::pair<ClassId, int>, int> blasted;

public:
    BlastField(bool nonFriendly,
               int stun,
               int magnitude,
               int ttl,
               int damage,
               const btVector3& origin,
               btCollisionShape* fieldshape,
               int team_id,
               btDiscreteDynamicsWorld* curworld);
    virtual ~BlastField();
    int HandleField() override;
};