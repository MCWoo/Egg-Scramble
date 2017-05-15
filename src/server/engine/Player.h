#pragma once
#include "Entity.h"
#include "network/GameData.h"

#include <algorithm> // std::min, std::max
#include <vector>

#ifdef max
    #undef max
#endif
#ifdef min
    #undef min
#endif

class Flag;
class PowerUp;
class Weapon;

class Player : public Entity
{
private:
    int m_teamId;
    std::string m_name;

    PosInfo m_position;
    std::vector<Flag*> m_flags;
    int m_jumpSem = 2;
    int m_stunCount = 0;
    Weapon* m_playerWeapon = nullptr;
    Weapon* m_peckWeapon = nullptr;
    bool m_alive = true;          // am i alive?
    unsigned int m_deathTime = 0; // when did i die?

    // Base and Bonus movement
    int m_baseJump = 40;
    int m_baseSpeed = 25;
    int m_speedPenalty = 0; // penalty on your speed
    int m_lowestSpeed = 12; // lower bound on speed
    int m_bonusJump = 0;
    int m_bonusSpeed = 0;
    int m_hitPoints = 100;
    int m_powerupDuration = 0;

    PowerUp* m_power = nullptr;

public:
    static const int maxHealth = 100;

    Player(int objectid, int teamid, PosInfo pos, btDiscreteDynamicsWorld* physicsWorld);

    ~Player();

    void SetCamAngle(float yos);

    // TODO *********************************
    void PrintPlayerVelocity();

    // Makes the player jump
    void JumpPlayer();

    // Resets player jump semaphore
    void ResetJump();

    // Flag handling
    void AcquireFlag(Flag* flag);

    void LoseFlags();

    // use weapon
    // Pass in current rotation and position
    void UseWeapon();

    void EquipWeapon(Weapon* newWeapon);

    void DiscardWeapon();
    void LosePower();

    bool HasPower();
    void EquipPower(PowerUp* powerup);
    void ResetPower() { m_power = nullptr; }
    // How much damage did they take, what is the world tick when they took this damage?
    int takeDamage(int damage, unsigned int world_tick);

    void UsePeck();

    // Which tick did this player die on? Disperses the player's flags then schedules his respawn
    // for later
    void HandleDeath(unsigned int death_tick);

    void Move(btVector3* changeVelocity) override;

    //////////////////////////////////////////////////////////////////////
    // Getters and Setters
    //////////////////////////////////////////////////////////////////////
    int GetTeamId();

    std::string GetName() const { return m_name; }
    void SetName(std::string n) { m_name = n; }
    PosInfo GetPosition() const { return m_position; };
    int GetScore() { return m_flags.size(); };
    int GetJumpSem() const { return m_jumpSem; }
    void SetJumpSem() { m_jumpSem = 0; }
    int GetStun() const { return m_stunCount; }
    void SetStun(int stun) { m_stunCount = stun; }
    bool HasWeapon() const;
    WeaponType GetPlayerWeaponType();
    Weapon* GetWeapon() { return m_playerWeapon; };
    bool IsAlive() const { return m_alive; }
    void SetAlive(bool a) { m_alive = a; }
    unsigned int GetDeathTime() const { return m_deathTime; }
    void SetDeathTime(unsigned int dt) { m_deathTime = dt; }
    int GetPlayerJump() const { return m_baseJump + m_bonusJump; };
    // Return Player Base + Bonus
    int GetPlayerSpeed() const
    {
        // check if you're too slow for the speed threshold
        return std::max(m_baseSpeed + m_bonusSpeed - m_speedPenalty, m_lowestSpeed);
        //return max(m_baseSpeed + m_bonusSpeed - m_speedPenalty, m_lowestSpeed);
    };

    int GetBonusJump() const { return m_bonusJump; }
    void SetBonusJump(int jump) { m_bonusJump = jump; }
    int GetBonusSpeed() const { return m_bonusSpeed; }
    void SetBonusSpeed(int speed) { m_bonusSpeed = speed; }
    int GetHp() { return m_hitPoints; };
    // TODO: Fix when you can define macro NOMINMAX
    void GainHP(int gain) { m_hitPoints = std::min(m_hitPoints + gain, maxHealth); }
    //void GainHP(int gain) { m_hitPoints = min(m_hitPoints + gain, maxHealth); }
    int GetPowerUpDuration() const { return m_powerupDuration; }
    void SetPowerUpDuration(int dur) { m_powerupDuration = dur; }
    int GetHitPoints() const { return m_hitPoints; }
    void SetHitPoints(int hp) { m_hitPoints = hp; }
    PowerUp* GetPower() { return m_power; }
};
