#pragma once

#include "Network/Serialization/Serializable.h"
#include "server/engine/ObjectId.h"

#include <string>
#include <cstdint>

namespace Constants
{
    const int worldWidth = 230;
    const int worldHeight = 500;
}

enum class MoveType
{
    Invalid = -1,
    Forward = 0,
    Backward = 1,
    Left = 2,
    Right = 3,
};

static const int NUM_COLLECT_TYPES = 2; // Types of collectables there are

enum class CollectType
{
    Weapon,
    PowerUp,
};

static const int NUM_WEAPON_TYPES = 6; // number of types of weapons there are

enum class WeaponType
{
    SeedGun,
    BounceGun,
    GrenadeLauncher,
    TeleportGun,
    BlastMine,
    Shotgun,
};

static const int NUM_POWER_TYPES = 2;

enum class PowerUpType
{
    HealthGain,
    JumpUp,
    SpeedUp,
};

enum class AttackType
{
    Peck,
    Weapon,
};

enum class GameDataId
{
    Position,
    Remove,
    Score,
    Emote,
    Name,
};

struct GameInfo : public Serializable
{
    GameDataId id;

    // NETWORKING NOTE:
    // Usually, you want to serialize into PacketData's buf before you send
    virtual void Serialize(std::uint8_t* data) override;
    virtual void Deserialize(std::uint8_t* data) override;
};

// Position info of object
struct PosInfo : GameInfo
{
    // general info
    int id; // client id
    int team_id;

    int skin;

    int oid;     // object id
    ClassId cid; // class id
    int sub_id; // this kind of id is used for subclasses, like different weapons spawning different
                // bullets

    // object coordinates
    float x;
    float y;
    float z;

    int hp;

    int direction; // client -> server move data

    // rotation coords
    float rotw;
    float rotx;
    float roty;
    float rotz;

    float yos;

    int num_eggs; // num eggs this player has
    int jump;     // jumping semaphore of the player

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};

// What needs to get removed
struct RemInfo : GameInfo
{
    int rem_oid;
    ClassId rem_cid;

    int rec_oid; // What was responsible for the removal, i.e. player 2 collects an egg, p2's info
                 // goes here
    ClassId rec_cid;
    int team_id;

    int sub_id;  // For removing collectable of type sub_id
    int sub_id2; // For removing subtype of collectable, like subtypes of the weapon or powerup

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};

// team scores
struct ScoreInfo : GameInfo
{
    int t0_score;
    int t1_score;

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};

// Who's emoting, also will include death
struct EmoteInfo : GameInfo
{
    int id;

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};

// Anything that just needs some ints, i.e. attacktype
struct MiscInfo : GameInfo
{
    int misc1;
    int misc2;
    float misc3;

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};

struct NameInfo : GameInfo
{
    int player_id;
    std::string name;

    void Serialize(std::uint8_t* data) override;
    void Deserialize(std::uint8_t* data) override;
};