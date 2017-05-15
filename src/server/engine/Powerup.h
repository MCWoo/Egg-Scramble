#pragma once
#include "network/GameData.h" // PowerUpType

class Player;

class PowerUp // powerup base class
{
protected:
    PowerUpType m_type;

public:
    PowerUpType GetType() { return m_type; }
    void SetType(PowerUpType type) { m_type = type; }
    virtual void ApplyPower(Player*){} // applies the powerup to the player
    virtual void RemovePower(Player*){}
};