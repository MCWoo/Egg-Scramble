#pragma once

#include "Entity.h"
#include "Network/GameData.h"

class Player;
class PowerUp;
class Weapon;

class Collectable : public Entity
{
private:
    CollectType m_type;
    Weapon* m_weapon;
    PowerUp* m_powerup;

public:
    void SetWeapon(Weapon* nw) { m_weapon = nw; }
    Weapon* GetWeapon() { return m_weapon; }
    PowerUp* GetPowerUp() { return m_powerup; }
    Collectable(int objectid, PosInfo pos, btDiscreteDynamicsWorld* curworld, Weapon* wp);
    Collectable(int objectid, PosInfo pos, btDiscreteDynamicsWorld* curworld, PowerUp* powerup);
    virtual ~Collectable();
    virtual void HandleCollect(Player* collidedPlayer);
    CollectType GetType() { return m_type; }
};
