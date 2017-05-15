#include "ServerGame.h"
#include "engine/Player.h"
#include "engine/World.h"

#include <algorithm>

unsigned int ServerGame::m_clientId;

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    m_clientId = 0;
    m_gameStarted = false;

    m_scores[0] = 0;
    m_scores[1] = 0;

    // set up the server m_network to listen
    m_network = new ServerNetwork();

    m_engine = new Engine();
}

ServerGame::~ServerGame(void)
{
}

void ServerGame::Update()
{
    // get new clients only if the game hasn't started
    if (!m_gameStarted)
    {
        if (m_network->AcceptNewClient(m_clientId))
        {
            printf("client %d has been connected to the server\n", m_clientId);

            // This will be an INIT_CONNECTION packet
            ReceiveFromClients();
        }
    }

    auto curr_time = std::chrono::steady_clock::now();

    auto fp_stamp = curr_time - m_startTime;

    int diff = fp_stamp.count();

    if (!m_gameOver && m_eggsSpawned && diff >= 300000)
    {
        m_gameOver = true;
        if (m_scores[0] == m_scores[1])
        {
            SendGameOverPacket(-1);
        }
        else if (m_scores[0] > m_scores[1])
        {
            SendGameOverPacket(0);
        }
        else
        {
            SendGameOverPacket(1);
        }
    }
    else
    {
        if (m_gameOver == false && m_eggsSpawned && ((diff % 10000) <= 16))
        {
            SendTimeStampPacket(diff);
        }
    }

    ReceiveFromClients();

    // Check that all clients are ready

    if (m_gameStarted && m_readyClients >= m_clientId)
    {
        if (m_gameOver)
            return;

        if (m_spawnedClients == m_readyClients && !m_eggsSpawned)
        {
            m_totalEggs = 2 * m_readyClients + 1;
            for (int i = 0; i < m_totalEggs; i++)
            {
                m_engine->SpawnRandomFlag();
            }
            m_eggsSpawned = true;
            Sleep(2000); // should wait for clients to respond
            m_startTime = std::chrono::steady_clock::now();
        }
        if (!m_engine->HasInitialSpawned())
        {
            m_engine->SendPreSpawn(m_readyClients);
        }

        // once eggs has spawned, everything has spawned and we can begin the world cycle
        auto t1 = std::chrono::steady_clock::now();

        if (m_eggsSpawned)
        {
            m_engine->GetWorld()->Update();
        }

        auto t2 = std::chrono::steady_clock::now();

        float thresh = 16.67f; // 60 FPS
        //float thresh = 33;   // 30 frames
        std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
        //("DIFFERENCE: %f\n", fp_ms.count());

        if (thresh - fp_ms.count() < 0)
        {
            printf("TIMING ERROR: %f\n", thresh - fp_ms.count());
        }
        else
        {
            // printf("SLEEPING: %f\n", (thresh - fp_ms.count()));

            Sleep((thresh - fp_ms.count()));

            auto t3 = std::chrono::steady_clock::now();

            std::chrono::duration<double, std::milli> fp_after = t3 - t1;

            //("TOTAL AFTER SLEEP: %f\n", fp_after.count());
        }
    }
}

void ServerGame::ReceiveFromClients()
{
    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for (iter = m_network->m_sessions.begin(); iter != m_network->m_sessions.end(); iter++)
    {
        int m_dataa_length = m_network->ReceiveData(iter->first, m_networkData);

        if (m_dataa_length <= 0)
        {
            // no m_dataa recieved
            continue;
        }

        int i = 0;
        while (i < (unsigned int)m_dataa_length)
        {
            // Deserialize and check the type of packet
            packet.Deserialize(&m_networkData[i]);

            switch (static_cast<PacketTypes>(packet.m_header.packet_type))
            {
            case PacketTypes::InitConnection:
                // offset is i here since we want to parse the packet m_header
                // Receive it and Send it right back
                ReceiveInitPacket(i);
                // SendInitPacket();
                // SendActionPackets();
                break;
            case PacketTypes::SetUsername:
                ReceiveNamePacket(i);
                break;
            case PacketTypes::JoinTeam:
                ReceiveJoinPacket(i);
                break;
            case PacketTypes::ReadyGame:
                m_readyClients++;
                // printf("ready clients: %d\nm_clientId: %d\n", m_readyClients, m_clientId);
                break;
            case PacketTypes::IndSpawnEvent:
                ReceiveIndSpawnPacket(i + sizeof(PacketHeader));
                m_spawnedClients++;
                break;
            case PacketTypes::StartGame:
                ReceiveStartPacket(i);
                Sleep(7000);
                SendStartPacket();
                break;
            case PacketTypes::MoveEvent:
                // Sends a move packet based on if reception was valid
                ReceiveMovePacket(i);
                break;
            case PacketTypes::JumpEvent:
                ReceiveJumpPacket(i);
                break;
            case PacketTypes::DanceEvent:
                ReceiveDancePacket(i);
                break;
            case PacketTypes::VRotationEvent:
                ReceiveRotationPacket(i + sizeof(PacketHeader));
                break;
            case PacketTypes::AttackEvent:
                ReceiveAttackPacket(i);
                break;
            case PacketTypes::DiscardEvent:
                ReceiveDiscardPacket(i);
                break;
            default:
                printf("error in packet types\n");
                break;
            }
            i += sizeof(Packet);
        }
    }
}

// Handle new init packet from client
void ServerGame::ReceiveInitPacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);

    printf("server Received init packet from client\n");
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::InitConnection);

    // Send back to the client and assign client id
    packet.m_header.m_receiver = m_clientId;
    packet.m_header.m_sender = SERVER_ID;

    packet.Serialize(packetData);

    m_network->SendToClient(packetData, packetSize, m_clientId);
    printf("server sent init packet to client %d\n", m_clientId);
    m_clientId++;
}

// Unused, we Send the init back right away in Receive
// We actually only want to Send this back to the client that we Received it from, but assume 1
// client for now
void ServerGame::SendInitPacket()
{
    printf("Sending init to clients\n");
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::InitConnection);

    packet.Serialize(packetData);

    m_network->SendToClient(packetData, packetSize, m_clientId);
}

void ServerGame::ReceiveJoinPacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);

    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset + sizeof(PacketHeader)]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&m_data->m_buffer);

    printf("recieved join packet from %d for %d\n", m_header->m_sender, pi->team_id);

    int client = m_header->m_sender;

    if (m_teamMap.find(client) == m_teamMap.end())
    { // if new client Send all client team m_dataa
        for (std::map<int, int>::iterator it = m_teamMap.begin(); it != m_teamMap.end(); it++)
        {
            SendJoinPacket(it->first);
        }
    }

    printf("(before) m_teamMap size = %d", m_teamMap.size());
    m_teamMap[client] = pi->team_id;
    printf("m_teamMap size = %d", m_teamMap.size());
    SendJoinPacket(client);
};

void ServerGame::SendJoinPacket(int client)
{
    const unsigned int packetSize = sizeof(Packet);

    // found
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::JoinTeam);

    PosInfo p;
    p.id = client;
    p.team_id = m_teamMap[client];

    printf("Sending join packet for client %d in team %d\n", client, p.team_id);

    packet.m_data.m_gameDataId = GameDataId::Position;

    p.Serialize(packet.m_data.m_buffer);
    packet.Serialize(packetData);

    packet.m_header.m_sender = SERVER_ID;

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
};

void ServerGame::ReceiveStartPacket(int offset)
{
    SendLoadPacket();

    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);

    printf("recieved start packet from %d\n", m_header->m_sender);
    if (!m_gameStarted)
    {
        printf("initializing world with %d players\n", m_clientId + 1);
        m_engine->InitWorld(m_clientId + 1);
        m_gameStarted = true;
    }
    /*else {
        printf("re-initializing world with %d players\n", m_clientId + 1);
        m_engine->InitWorld(m_clientId + 1);
    }*/

    // add player
}

void ServerGame::SendStartPacket()
{ // will add more later based on generated world
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::StartGame);

    PosInfo p;
    // p.id = m_clientId + 1;

    packet.m_data.m_gameDataId = GameDataId::Position;

    printf("Sending start packet with m_clientId %d\n", m_clientId + 1);

    p.Serialize(packet.m_data.m_buffer);
    packet.Serialize(packetData);

    packet.m_header.m_sender = SERVER_ID;

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendLoadPacket()
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::ServerLoading);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendReadyToSpawnPacket()
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::ReadyToSpawnEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveIndSpawnPacket(int offset)
{
    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&m_data->m_buffer);

    m_engine->SpawnRandomPlayer(pi->id, pi->team_id, pi->skin);
}

void ServerGame::SendSpawnPacket(PosInfo pi)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::SpawnEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Position;
    pi.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendRemovePacket(ClassId rem_cid, int rem_oid)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::RemoveEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Remove;

    RemInfo r;
    r.rem_cid = rem_cid;
    r.rem_oid = rem_oid;

    r.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendRemovePacket(ClassId rem_cid, int rem_oid, ClassId rec_cid, int rec_oid)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::RemoveEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Remove;

    RemInfo r;
    r.rem_cid = rem_cid;
    r.rem_oid = rem_oid;
    r.rec_cid = rec_cid;
    r.rec_oid = rec_oid;

    r.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendDiscardPacket(int id)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::DiscardEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    MiscInfo m;
    m.misc1 = id;

    m.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendRemovePacket(ClassId rem_cid,
                                  int rem_oid,
                                  ClassId rec_cid,
                                  int rec_oid,
                                  CollectType c_type,
                                  int subtype)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::RemoveEvent);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Remove;

    RemInfo r;
    r.rem_cid = rem_cid;
    r.rem_oid = rem_oid;
    r.rec_cid = rec_cid;
    r.rec_oid = rec_oid;
    r.sub_id = static_cast<int>(c_type);
    r.sub_id2 = subtype;

    r.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveMovePacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset + sizeof(PacketHeader)]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&m_data->m_buffer);
    Entity* ent = reinterpret_cast<Entity*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));

    Player* player =
        reinterpret_cast<Player*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));

    // don't take client input if stunned
    if (player->GetStun() > 0)
        return;

    switch (static_cast<MoveType>(pi->direction))
    {
    case MoveType::Forward:
    {
        btVector3 curVec = player->GetEntityVelocity();
        curVec.setZ(player->GetPlayerSpeed());
        curVec.setX(0);
        ent->Move(&curVec);
        break;
    }
    case MoveType::Backward:
    {
        btVector3 curVec = player->GetEntityVelocity();
        curVec.setZ(-player->GetPlayerSpeed());
        curVec.setX(0);
        ent->Move(&curVec);
        break;
    }
    case MoveType::Left:
    {
        btVector3 curVec = player->GetEntityVelocity();
        curVec.setX(player->GetPlayerSpeed());
        curVec.setZ(0);
        ent->Move(&curVec);
        break;
    }
    case MoveType::Right:
    {
        btVector3 curVec = player->GetEntityVelocity();
        curVec.setX(-player->GetPlayerSpeed());
        curVec.setZ(0);
        ent->Move(&curVec);
        break;
    }
    }
}

void ServerGame::SendMovePacket(ClassId class_id, int obj_id)
{
    Entity* ent = reinterpret_cast<Entity*>(EntitySpawner::Instance()->GetEntity(class_id, obj_id));

    // If this is not a player and the player is dead, don't update
    if (class_id == ClassId::Player && !((Player*)ent)->IsAlive())
    {
        return;
    }

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::MoveEvent);

    PosInfo p;
    packet.m_data.m_gameDataId = GameDataId::Position;

    // Extract the vector and Send it with the posinfo object
    btVector3 vec = ent->GetEntityPosition();

    p.cid = class_id;
    p.oid = obj_id;
    p.x = vec.getX();
    p.y = vec.getY();
    p.z = vec.getZ();

    if (class_id == ClassId::Player)
    {
        p.num_eggs = ((Player*)ent)->GetScore();
        p.jump = ((Player*)ent)->GetJumpSem();
        p.hp = ((Player*)ent)->GetHp();
    }

    p.Serialize(packet.m_data.m_buffer);

    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveRotationPacket(int offset)
{
    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&m_data->m_buffer);

    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset - sizeof(PacketHeader)]);

    // All rotation packets will be player type, since it's from client
    Entity* ent = reinterpret_cast<Entity*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));

    ent->SetEntityRotation(0, pi->roty, 0, pi->rotw);
    SendRotationPacket(ClassId::Player, m_header->m_sender);
}

void ServerGame::SendRotationPacket(ClassId class_id, int obj_id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Entity* ent = reinterpret_cast<Entity*>(EntitySpawner::Instance()->GetEntity(class_id, obj_id));

    // If this is not a player and the player is dead, don't update
    if (class_id == ClassId::Player && !((Player*)ent)->IsAlive())
    {
        return;
    }

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::VRotationEvent);

    packet.m_data.m_gameDataId = GameDataId::Position;

    PosInfo p;
    p.cid = (ClassId)class_id;
    p.oid = obj_id;

    // why do we switch w and y and multiply by negative, we don't know but it fixes it
    btQuaternion q = ent->GetEntityRotation();
    btVector3 v = ent->GetEntityPosition();
    p.x = v.getX();
    p.y = v.getY();
    p.z = v.getZ();
    p.rotw = q.getW();
    p.rotx = q.getX();
    p.roty = q.getY();
    p.rotz = q.getZ();

    p.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveJumpPacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);

    Player* player =
        reinterpret_cast<Player*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));

    // ignore client input if stunned
    if (player->GetStun() > 0)
        return;

    player->JumpPlayer();
}

void ServerGame::SendScorePacket()
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::UpdateScore);

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Score;

    ScoreInfo s;
    s.t0_score = m_scores[0];
    s.t1_score = m_scores[1];

    printf("Sending score packet: %d, %d\n", s.t0_score, s.t1_score);
    s.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendGameOverPacket(int winner)
{
    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::GameOver);
    m_gameOver = true;

    const unsigned int packetSize = sizeof(Packet);

    std::uint8_t packetData[packetSize];

    packet.m_data.m_gameDataId = GameDataId::Score;

    ScoreInfo s;
    s.t0_score = m_scores[0];
    s.t1_score = m_scores[1];

    printf("Sending game over\n");
    s.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveAttackPacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset + sizeof(PacketHeader)]);
    MiscInfo* m = reinterpret_cast<MiscInfo*>(&m_data->m_buffer);
    Player* player =
        reinterpret_cast<Player*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));

    player->SetCamAngle(m->misc3);

    if (static_cast<AttackType>(m->misc1) == AttackType::Weapon)
    {
        player->UseWeapon();
    }
    else if (static_cast<AttackType>(m->misc1) == AttackType::Peck)
    {
        player->UsePeck();
    }
}

void ServerGame::SendTimeStampPacket(int diff)
{
    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::TimeEvent);

    MiscInfo m;
    m.misc1 = (diff / 1000);

    m.Serialize(packet.m_data.m_buffer);

    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    packet.Serialize(packetData);

    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendAttackPacket(int id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::AttackEvent);

    packet.m_data.m_gameDataId = GameDataId::Emote;

    EmoteInfo e;
    e.id = id;
    e.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveDiscardPacket(int offset)
{
    // PacketHeader* m_header = (PacketHeader *) &(m_networkData[offset]);

    // shared_ptr<Player> player = m_engine->GetWorld()->GetPlayer(m_header->m_sender);
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    Player* player =
        reinterpret_cast<Player*>(EntitySpawner::Instance()->GetEntity(ClassId::Player, m_header->m_sender));
    player->DiscardWeapon();
}

void ServerGame::ReceiveDancePacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    SendDancePacket(m_header->m_sender);
}

void ServerGame::SendDancePacket(int id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::DanceEvent);

    packet.m_data.m_gameDataId = GameDataId::Emote;

    EmoteInfo e;
    e.id = id;
    e.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendDeathPacket(int id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::DeathEvent);

    packet.m_data.m_gameDataId = GameDataId::Emote;

    EmoteInfo e;
    e.id = id;
    e.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendRespawnPacket(int id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::RespawnEvent);

    packet.m_data.m_gameDataId = GameDataId::Emote;

    EmoteInfo e;
    e.id = id;
    e.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::SendNamePacket(int player_id)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::SetUsername);

    packet.m_data.m_gameDataId = GameDataId::Name;

    NameInfo n;
    n.player_id = player_id;
    n.name = m_nameMap.at(player_id);
    n.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    m_network->SendToAll(packetData, packetSize);
}

void ServerGame::ReceiveNamePacket(int offset)
{
    PacketHeader* m_header = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    PacketData* m_data = reinterpret_cast<PacketData*>(&m_networkData[offset + sizeof(PacketHeader)]);
    NameInfo* n = reinterpret_cast<NameInfo*>(&m_data->m_buffer);

    m_nameMap[m_header->m_sender] = n->name;
    /*Player* player = reinterpret_cast<Player*>(EntitySpawner::Instance()->GetEntity(ClassId::Player,
    m_header->m_sender));
    player->SetName(n->name);*/

    SendNamePacket(m_header->m_sender);
}