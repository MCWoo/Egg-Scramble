#pragma once
#include "GameData.h"
#include "Network/Serialization/Serializable.h"

#include <string.h>

const int g_maxPacketSize = 1000000;
const int g_dataSize = 100; // Change this to the largest data size that is needed

enum class PacketTypes
{
    InitConnection, // Initial connection setup
    SetUsername,
    JoinTeam,
    StartGame,     // A client would send this to start the game
    ServerLoading, // server sends to client
    ReadyGame,     // The clients send this to say they're ready
    ReadyToSpawnEvent,
    IndSpawnEvent,
    SpawnEvent,  // Creating a new object, sent to the client
    RemoveEvent, // Removing an object, sent to the client
    MoveEvent,
    TimeEvent,
    VRotationEvent,
    JumpEvent,
    AttackEvent,
    DiscardEvent,
    DanceEvent,
    DeathEvent,
    RespawnEvent,
    UpdateScore,
    GameOver,
};

struct PacketHeader
{
    unsigned int packet_type;

    // id for server will be -1
    int m_sender;
    int m_receiver;
};

struct PacketData
{
    GameDataId m_gameDataId;
    std::uint8_t m_buffer[g_dataSize];
};

struct Packet : public Serializable
{
    PacketHeader m_header;
    PacketData m_data;

    void Serialize(std::uint8_t* data) override { memcpy(data, this, sizeof(Packet)); }
    void Deserialize(std::uint8_t* data) override { memcpy(this, data, sizeof(Packet)); }
};