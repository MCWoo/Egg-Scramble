#include "ClientGame.h"

#include <cstdio>

#include <chrono>
#include <ratio>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "Graphics/Scene.h"
#include "network/GameData.h"
#include "network/NetworkData.h"
#include "TextRenderer.h"
#include "client/PlayState.h"
#include "client/LobbyState.h"
#include "client/GameOverState.h"
#include "ConfigManager.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/ModelManager.h"
#include "Audio/SoundBufferManager.h"

const std::string ClientGame::EventQuit = "Quit";
const std::string ClientGame::EventJump = "Jump";
const std::string ClientGame::EventWeaponAttack = "Weapon_Attack";
const std::string ClientGame::EventPeckAttack = "Peck_Attack";
const std::string ClientGame::EventDiscardWeapon = "Discard_Weapon";
const std::string ClientGame::EventStart = "Start";
const std::string ClientGame::EventMoveForward = "Move_Forward";
const std::string ClientGame::EventMoveBackward = "Move_Backward";
const std::string ClientGame::EventMoveLeft = "Move_Left";
const std::string ClientGame::EventMoveRight = "Move_Right";
const std::string ClientGame::EventScoreboard = "Scoreboard";
const std::string ClientGame::EventTauntDance = "Taunt_Dance";
const std::string ClientGame::EventTauntDeath = "Taunt_Death";
const std::string ClientGame::EventTauntPeck = "Taunt_Peck";

////////////////////////////////////////////////////////////////////////////////
int ClientGame::PlaySound(const std::string& soundName, SoundsHandler::SoundOptions options)
{
    std::shared_ptr<sf::SoundBuffer> soundBuffer = SoundBufferManager::GetSoundBuffer(soundName);
    if (soundBuffer)
    {
        return m_soundsHandler.Play(*soundBuffer, options);
    }
    return -1;
}

bool ClientGame::StopSound(int index)
{
    return m_soundsHandler.StopSound(index);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::PlayMenuSound()
{
    std::shared_ptr<sf::SoundBuffer> soundBuffer = SoundBufferManager::GetSoundBuffer("Menu");
    if (soundBuffer)
    {
        // loop and set to origin
        SoundsHandler::SoundOptions options;
        options.m_loops = true;
        options.m_isRelativeToListener = true;
        m_menuSound = m_soundsHandler.Play(*soundBuffer, options);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::StopMenuSound()
{
    if (m_menuSound != -1)
    {
        m_soundsHandler.StopSound(m_menuSound);
        m_menuSound = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////
ClientGame::ClientGame(void) : m_soundsHandler()
{
#ifdef _WIN32
    m_network = new ClientNetwork();

    m_startSent = false;

    SendInitPacket();
    // sendStartPacket(); // temp - will add start button

    Initialize();

#endif
}

////////////////////////////////////////////////////////////////////////////////
ClientGame::~ClientGame(void)
{
    Destroy();
}

#ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////
// Do we want to create a new world every time we get a new init packet
void ClientGame::ReceiveInitPacket(int offset)
{
    PacketHeader* hdr = reinterpret_cast<PacketHeader*>(&(m_networkData[offset]));

    // Find out what our client id is from the init packet of the server
    m_clientId = hdr->m_receiver;

    printf("set client id to %d\n", m_clientId);

    // Initialize();
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendInitPacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    // no client id yet
    packet.m_header.m_receiver = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::InitConnection);

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveJoinPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&dat->m_buffer);

    // set our team if it's for us
    if (pi->id == m_clientId)
    {
        m_clientTeam = pi->team_id;
        printf("setting client team to %d for player %d\n", m_clientTeam, m_clientId);
    }

    printf("receiveJoinPacket for player %d on team %d\n", pi->id, pi->team_id);
    int player = pi->id;
    int team = pi->team_id;

    if (team == 0)
    {
        m_team0.erase(std::remove(m_team0.begin(), m_team0.end(), player),
                      m_team0.end()); // erase from both lists
        m_team1.erase(std::remove(m_team1.begin(), m_team1.end(), player), m_team1.end());

        m_team0.push_back(player);
    }
    else
    {
        m_team0.erase(std::remove(m_team0.begin(), m_team0.end(), player), m_team0.end());
        m_team1.erase(std::remove(m_team1.begin(), m_team1.end(), player), m_team1.end());

        m_team1.push_back(player);
    }
};

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendJoinPacket(int team)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;

    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::JoinTeam);

    PosInfo pi;
    pi.id = m_clientId;
    pi.team_id = team;
    pi.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
};

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendReadyPacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;

    packet.m_header.packet_type = static_cast<int>(PacketTypes::ReadyGame);
    packet.m_header.m_sender = m_clientId;

    printf("sending a ready packet\n");

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveStartPacket(int offset)
{
    PacketHeader* hdr = reinterpret_cast<PacketHeader*>(&(m_networkData[offset]));
    PacketData* dat =
        reinterpret_cast<PacketData*>(&(m_networkData[offset + sizeof(PacketHeader)]));

    PosInfo* pi = reinterpret_cast<PosInfo*>(&(dat->m_buffer));
    printf("received start packet for %d players\n", pi->id);

    Window::m_pStateManager->ChangeState(
        CPlayState::GetInstance(Window::m_pStateManager)); // start game

    m_gameStarted = true;
    m_totalEggs = (m_team0.size() + m_team1.size()) * 2 + 1;
    SendReadyPacket();
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendStartPacket()
{
    if (m_startSent)
        return;

    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::StartGame);

    packet.Serialize(packetData);

    m_startSent = true;
    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
// sendIndSpawnPacket
void ClientGame::ReceiveReadyToSpawnPacket(int offset)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::IndSpawnEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    PosInfo pi;
    pi.id = m_clientId;
    pi.team_id = m_clientTeam;
    printf("send IndSpawn Packet for player %d on team %d\n", m_clientId, pi.team_id);
    pi.skin = rand() % 3;

    pi.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveSpawnPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* p = reinterpret_cast<PosInfo*>(dat->m_buffer);

    // spawn the thing
    Scene::Instance()->AddEntity((*p));

    if (!m_iSpawned && p->oid == m_clientId)
    {
        m_iSpawned = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SetName(std::string name)
{
    if (name == "Enter your name" || name == "")
    {
        std::string def;
        def += "Player ";
        def += std::to_string(m_clientId);
        m_nameMap[m_clientId] = def;
    }
    else
        m_nameMap[m_clientId] = name;
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveRemovePacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    RemInfo* r = reinterpret_cast<RemInfo*>(&dat->m_buffer);

    Scene::Instance()->RemoveEntity(r->rem_cid, r->rem_oid);

    // if it's a flag, then a player should've removed it
    if (r->rem_cid == ClassId::Flag)
    {
        // change the player's score based on that flag
        if (r->rec_cid == ClassId::Player)
        {
            IncScore(((Player*)(Scene::Instance()->GetEntity(ClassId::Player, r->rec_oid).get()))
                         ->GetTeam(),
                     1);
        }
        glm::vec3 position =
            ((Player*)(Scene::Instance()->GetEntity(ClassId::Player, r->rec_oid).get()))
                ->Position();
        SoundsHandler::SoundOptions options(
            position.x, position.y, position.z); // Play at own position
        PlaySound("Collect_Egg", options);
    }
    if (r->rem_cid == ClassId::Collectable
        && static_cast<CollectType>(r->sub_id) == CollectType::Weapon) // If Weapon/collectable
    {
        // Tell the client you got a weapon!
        if (m_clientId == r->rec_oid)
        {
            if (Scene::Instance()->GetPlayer()->GetWeapon() == -1)
                Scene::Instance()->GetPlayer()->SetWeapon(r->sub_id2);
        }
        // make sound regardless of who it is
        glm::vec3 position =
            ((Player*)(Scene::Instance()->GetEntity(ClassId::Player, r->rec_oid).get()))
                ->Position();
        SoundsHandler::SoundOptions options(
            position.x, position.y, position.z); // Play at own position
        PlaySound("Collect_Weapon", options);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveMovePacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&dat->m_buffer);

    // if(pi->oid == 0)
    // printf("received move packet for obj id %d. Its coordinates are: %f, %f, %f\n", pi->oid,
    // pi->x, pi->y, pi->z);

    Scene::Instance()->GetEntity(pi->cid, pi->oid)->MoveTo(pi->x, pi->y, pi->z);

    // check the jump animation
    if (pi->cid == ClassId::Player)
    {
        Scene::Instance()->GetEntity(pi->cid, pi->oid)->SetScore(pi->num_eggs);
        if (pi->jump == 0 || pi->jump == 1)
            ((Player*)(Scene::Instance()->GetEntity(pi->cid, pi->oid).get()))->Jump();
    }

    // update hp
    ((Player*)(Scene::Instance()->GetEntity(pi->cid, pi->oid).get()))->SetHealth(pi->hp);
}

////////////////////////////////////////////////////////////////////////////////
// Need to know what direction to move in
void ClientGame::SendMovePacket(MoveType direction)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::MoveEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    PosInfo pi;
    pi.direction = static_cast<int>(direction);
    glm::quat rot = Scene::Instance()->GetPlayer()->Orientation();
    pi.rotw = rot.w;
    pi.rotx = rot.x;
    pi.roty = rot.y;
    pi.rotz = rot.z;
    pi.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);
    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveRotationPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    PosInfo* pi = reinterpret_cast<PosInfo*>(&dat->m_buffer);

    // printf("received a rotation packet with: %f, %f, %f, %f\n", pi->rotw, pi->rotx, pi->roty,
    // pi->rotz);

    // std::shared_ptr<Player> target = FindTarget(pi->id);
    // server don't tell us how we rotate
    if (pi->oid != m_clientId)
    {
        Scene::Instance()
            ->GetEntity(pi->cid, pi->oid)
            ->RotateTo(pi->rotw, pi->rotx, pi->roty, pi->rotz);
        glm::quat quat = Scene::Instance()->GetEntity(pi->cid, pi->oid)->Orientation();
    }

    // Rotate it if it's not a player
    if (pi->cid != ClassId::Player)
    {
        Scene::Instance()
            ->GetEntity(pi->cid, pi->oid)
            ->RotateTo(pi->rotw, pi->rotx, pi->roty, pi->rotz);
    }

    // left/right rotation
    /*glm::mat4 newToWorld = target->GetToWorld() * glm::rotate(glm::mat4(1.0f), pi->v_rotation,
    glm::vec3(0.0f, 1.0f, 0.0f));
    target->SetToWorld(newToWorld);

    // up/down rotation
    float newAngle = target->GetCamAngle() + pi->h_rotation;
    const static float pi2 = glm::pi<float>() / 2;
    target->SetCamAngle((newAngle > pi2) ? pi2 : ((newAngle < -pi2) ? -pi2 : newAngle));*/
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendRotationPacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::VRotationEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    // send the rotation of the main player
    PosInfo pi;
    glm::quat rot = Scene::Instance()->GetPlayer()->Orientation();
    pi.rotw = rot.w;
    pi.rotx = rot.x;
    pi.roty = rot.y;
    pi.rotz = rot.z;

    // pi.h_rotation = h_rot;
    pi.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendJumpPacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::JumpEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveScorePacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    ScoreInfo* s = reinterpret_cast<ScoreInfo*>(&dat->m_buffer);

    printf("received a score packet t0: %d   t1: %d\n", s->t0_score, s->t1_score);

    m_scores[0] = s->t0_score;
    m_scores[1] = s->t1_score;
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveGameOverPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    ScoreInfo* s = reinterpret_cast<ScoreInfo*>(&dat->m_buffer);

    if (s->t0_score == s->t1_score)
    {
        m_winner = -1;
    }
    else if (s->t0_score > s->t1_score)
    { // t0 win
        m_winner = 0;
    }
    else
    { // t1 win
        m_winner = 1;
    }

    if (m_winner == -1)
        printf("Game was a tie!\n");
    else
        printf("Team %d won!\n", m_winner);
    // change state to game over screen
    Window::m_pStateManager->ChangeState(GOState::GetInstance(Window::m_pStateManager));
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveTimeStampPacket(int offset)
{
    PacketData* data = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    MiscInfo* m = reinterpret_cast<MiscInfo*>(&data->m_buffer);
    if (!m_countdown)
    {
        m_startTime = std::chrono::steady_clock::now();
        m_countdown = 300 - m->misc1;
    }

    if (m_countdown != m->misc1)
    {
        m_countdown = 300 - m->misc1;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendAttackPacket(AttackType t)
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::AttackEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    MiscInfo m;
    m.misc1 = static_cast<int>(t);
    m.misc3 = Scene::Instance()->GetPlayer()->GetCamAngle();
    m.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveAttackPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    EmoteInfo* e = reinterpret_cast<EmoteInfo*>(&dat->m_buffer); // later parse this to miscinfo
    auto player =
        reinterpret_cast<Player*>(Scene::Instance()->GetEntity(ClassId::Player, e->id).get());
    player->Attack();
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendDiscardPacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::DiscardEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveDiscardPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&(m_networkData[offset]));
    MiscInfo* m = reinterpret_cast<MiscInfo*>(&dat->m_buffer);
    if (m->misc1 == m_clientId)
    {
        Scene::Instance()->GetPlayer()->SetWeapon(-1);
    }
}

////////////////////////////////////////////////////////////////////////////////
//	NOTE: We're going to use the sender id as the guy that's dancing instead of changing
void ClientGame::ReceiveDancePacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    EmoteInfo* e = reinterpret_cast<EmoteInfo*>(&dat->m_buffer);
    auto player =
        reinterpret_cast<Player*>(Scene::Instance()->GetEntity(ClassId::Player, e->id).get());
    player->Dance();
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendDancePacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::DanceEvent);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveDeathPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset]);
    EmoteInfo* e = reinterpret_cast<EmoteInfo*>(&dat->m_buffer);

    // model
    auto player =
        reinterpret_cast<Player*>(Scene::Instance()->GetEntity(ClassId::Player, e->id).get());
    player->Die();

    // score
    DecScore(player->GetTeam(), player->GetScore());

    // death overlay
    if (e->id == m_clientId)
    {
        CPlayState::GetInstance(Window::m_pStateManager)->Die();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveRespawnPacket(int offset)
{
    PacketData* dat = reinterpret_cast<PacketData*>(m_networkData[offset]);
    EmoteInfo* e = reinterpret_cast<EmoteInfo*>(&dat->m_buffer);
    auto player =
        reinterpret_cast<Player*>(Scene::Instance()->GetEntity(ClassId::Player, e->id).get());
    player->SetAlive(true);

    // update state
    if (e->id == m_clientId)
    {
        CPlayState::GetInstance(Window::m_pStateManager)->Respawn();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::SendNamePacket()
{
    const unsigned int packetSize = sizeof(Packet);
    std::uint8_t packetData[packetSize];

    Packet packet;
    packet.m_header.packet_type = static_cast<int>(PacketTypes::SetUsername);
    packet.m_header.m_sender = m_clientId;
    packet.m_header.m_receiver = SERVER_ID;

    NameInfo n;
    n.player_id = m_clientId;
    n.name = m_nameMap.at(m_clientId);
    n.Serialize(packet.m_data.m_buffer);

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);

    packet.Serialize(packetData);

    NetworkServices::Send(m_network->m_connectSocket, packetData, packetSize);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ReceiveNamePacket(int offset)
{
    PacketHeader* hdr = reinterpret_cast<PacketHeader*>(&m_networkData[offset]);
    PacketData* dat = reinterpret_cast<PacketData*>(&m_networkData[offset + sizeof(PacketHeader)]);
    NameInfo* n = reinterpret_cast<NameInfo*>(&dat->m_buffer);

    m_nameMap[n->player_id] = n->name;
    // Player* player =
    // reinterpret_cast<Player*>(Scene::Instance()->GetEntity(ClassId::Player,n->player_id).get());
    // player->SetName(n->name);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Update()
{
    Packet packet;
    int data_length = m_network->ReceivePackets(m_networkData);

    if (data_length <= 0)
    {
        return; // no data recieved
    }

    unsigned int i = 0;
    while (i < (unsigned int)data_length)
    {
        packet.Deserialize(&m_networkData[i]);

        switch (static_cast<PacketTypes>(packet.m_header.packet_type))
        {
        case PacketTypes::InitConnection:
            // offset for this will be the packet header
            ReceiveInitPacket(i);
            break;

        case PacketTypes::SetUsername:
            ReceiveNamePacket(i);
            break;

        case PacketTypes::JoinTeam:
            ReceiveJoinPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::StartGame:
            ReceiveStartPacket(i);
            break;

        case PacketTypes::ServerLoading:
            LobbyState::GetInstance(Window::m_pStateManager)->ServerLoading();
            break;

        case PacketTypes::ReadyToSpawnEvent:
            ReceiveReadyToSpawnPacket(i);
            break;

        case PacketTypes::SpawnEvent:
            // You want to offset the packet header
            ReceiveSpawnPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::RemoveEvent:
            ReceiveRemovePacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::DiscardEvent:
            ReceiveDiscardPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::MoveEvent:
            if (m_gameStarted) // the game needs to start for the client before this can happen
            {
                ReceiveMovePacket(i + sizeof(PacketHeader));
            }
            break;

        case PacketTypes::TimeEvent:
            ReceiveTimeStampPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::DanceEvent:
            ReceiveDancePacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::DeathEvent:
            ReceiveDeathPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::RespawnEvent:
            ReceiveRespawnPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::VRotationEvent:
            ReceiveRotationPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::AttackEvent:
            ReceiveAttackPacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::UpdateScore:
            ReceiveScorePacket(i + sizeof(PacketHeader));
            break;

        case PacketTypes::GameOver:
            ReceiveGameOverPacket(i + sizeof(PacketHeader));
            break;

        default:
            printf("error in packet types %d\n", packet.m_header.packet_type);
            break;
        }
        i += sizeof(Packet);
    }
}
#endif // ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Initialize()
{
    // Create the GLFW m_window
    m_window = Window::Create_window(1366, 768);

    // Print OpenGL and GLSL versions
    Print_versions();
    // Setup callbacks
    Setup_callbacks();
    // Setup OpenGL settings, including lighting, materials, etc.
    Setup_opengl_settings();

    // Initialize the shaders. Must be loaded before Loading screen to show the screen.
    ShaderManager::Instance()->LoadShaders();

    // show loading screen
    ShowLoadingScreen();

    ModelManager::Instance()->LoadModels();
    SoundBufferManager::Instance()->LoadSoundBuffers();
    // Initialize objects/pointers for rendering
    Window::Initialize_objects();

    TextRenderer::Initialize();
    Scene::Initialize();

    // go to login menu
    Window::m_pStateManager->ChangeState(CMenuState::GetInstance(Window::m_pStateManager));
    sf::Listener::setUpVector(0.0f, 1.0f, 0.0f); // Initialize the up vector
    for (int i = 0; i < SoundsHandler::MAX_SOUNDS; i++)
    {
        m_soundsHandler.SetMinDistance(i, ConfigManager::GetAsFloat("Sounds_Min_Distance"));
        m_soundsHandler.SetAttenuation(i, ConfigManager::GetAsFloat("Sounds_Attenuation"));
    }
    PlayMenuSound();

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    srand(static_cast<unsigned int>(time(NULL)));
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::ShowLoadingScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.28f, 0.65f, 0.89f, 1.0f); // reset color
    SpriteRenderer sprite_renderer = SpriteRenderer();
    Texture bg = Texture(GL_TEXTURE_2D, "assets/ui/loading/loading.png");
    int x = Texture::GetWindowCenter(bg.Width());
    int y = Window::s_height / 2 - bg.Height() / 2;

    sprite_renderer.DrawSprite(&bg,
                                glm::vec2(x, y),
                                glm::vec2(bg.Width(), bg.Height()),
                                0.0f,
                                glm::vec3(1.0f, 1.0f, 1.0f));

    glfwSwapBuffers(m_window);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Destroy()
{
    Window::Clean_up();
    // Destroy the m_window
    glfwDestroyWindow(m_window);
    // Terminate GLFW
    glfwTerminate();
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::GameLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
#ifdef _WIN32
        Update();
#endif
        auto curr_time = std::chrono::steady_clock::now();

        auto fp_stamp = curr_time - m_startTime;

        int diff = (int)fp_stamp.count();

        diff = diff / 1000;

        if ((300 - diff) == (m_countdown - 1))
        {
            m_countdown = 300 - diff;
        }

        // Measure speed
        PrintFrameRate();

        CheckController();

        // Main render display callback. Rendering of objects is done here.
        Window::Display_callback(m_window);
        // Idle callback. Updating objects, etc. can be done here.
        Window::Idle_callback();

        if (++m_tick % 15 == 0 && m_iSpawned)
        {
            ClientGame::Instance()->SendRotationPacket();
            m_tick = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Error_callback(int error, const char* description)
{
    // Print error
    fputs(description, stderr);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::LoadConfigs()
{
    ConfigManager::Instance()->LoadConfigs("eggs.cfg");
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Setup_callbacks()
{
    // Set the error callback
    glfwSetErrorCallback(ClientGame::Error_callback);
    // Set the key callback
    glfwSetKeyCallback(this->m_window, Window::Key_callback);

    glfwSetCursorPosCallback(m_window, Window::Mouse_callback);
    glfwSetMouseButtonCallback(m_window, Window::Mouse_button_callback);
    glfwSetCharCallback(m_window, Window::Char_callback);
    // Set the m_window resize callback
    glfwSetWindowSizeCallback(m_window, Window::Resize_callback);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Setup_glew()
{
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Setup_opengl_settings()
{
    // Setup GLEW
    Setup_glew();
    // Enable depth buffering
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon
    // You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Disable backface culling to render both sides of polygons
    glDisable(GL_CULL_FACE);
    // Set clear color
    glClearColor(0.28f, 0.65f, 0.89f, 1.0f);

    // Font Rendering
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

////////////////////////////////////////////////////////////////////////////////
void ClientGame::Print_versions()
{
    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

// If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::printf("Supported GLSL version is %s.\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Prints once per 5 seconds
void ClientGame::PrintFrameRate()
{
    double currentTime = glfwGetTime();
    m_nbFrames++;
    if (currentTime - m_lastTime >= 5.0) // If last prinf() was more than 1 sec ago
    {
        printf("%f ms/frame\n", 5000.0 / double(m_nbFrames)); // printf and reset timer
        m_nbFrames = 0;
        m_lastTime += 5.0;
    }
}

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Checks the controller input on Microsoft PC-joystick driver
// axis[0] = (left) right
// axis[1] = (left) down
// axis[2] = (< 0) Left trigger, (> 0) Right trigger
// axis[3] = (right) down
// axis[4] = (right) right
// button[0-3] = a, b, x, y
// button[4-5] = LB, RB
// button[6-7] = Back, Start
// button[8-9] = left/right analog
// button[10-13] = d-pad up, right, down, left
void ClientGame::CheckController()
{
    using namespace Controller;
    if (!glfwJoystickPresent(GLFW_JOYSTICK_1))
    {
        return;
    }

    // Get axes and buttons
    int axesCount, buttonCount;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

    // Check if we're starting the game
    if (!ClientGame::Instance()->HasStarted())
    {
        if (buttons[static_cast<int>(Buttons::Start)]) // Start
        {
            printf("client will send start game\n");
            printf("sending start packet\n");
            ClientGame::Instance()->SendStartPacket();
        }
        return;
    }
    else
    {
        // Handle analog movement
        HandleLeftAnalog(axes);
        HandleRightAnalog(axes);
        HandleButtonPress(buttons);
    }
}

////////////////////////////////////////////////////////////////////////////////
// axis[2] = (< 0) Left trigger, (> 0) Right trigger
void ClientGame::HandleTriggers(const float* axes)
{
    static const float threshold = 0.5f;
    if (axes[2] < -threshold)
    {
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_L_Trigger"));
    }
    if (axes[2] > threshold)
    {
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_R_Trigger"));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Handles View changing
void ClientGame::HandleRightAnalog(const float* axes)
{
    using namespace Controller;
    const float rotThreshold = 0.2f;
    if (abs(axes[static_cast<int>(Axes::RHorizontal)]) > rotThreshold
        || abs(axes[static_cast<int>(Axes::RVertical)]) > rotThreshold)
    {
        Scene::Instance()->GetPlayer()->ProcessViewMovement(
            abs(axes[static_cast<int>(Axes::RHorizontal)]) > rotThreshold
                ? axes[static_cast<int>(Axes::RHorizontal)]
                : 0.0f,
            abs(axes[static_cast<int>(Axes::RVertical)]) > rotThreshold
                ? -axes[static_cast<int>(Axes::RVertical)]
                : 0.0f);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Handles movement
void ClientGame::HandleLeftAnalog(const float* axes)
{
    using namespace Controller;
    const float threshold = 0.7f;
    int greatestAxis = abs(axes[static_cast<int>(Axes::LHorizontal)])
                               > abs(axes[static_cast<int>(Axes::LVertical)])
                           ? static_cast<int>(Axes::LHorizontal)
                           : static_cast<int>(Axes::LVertical);
    if (abs(axes[greatestAxis]) > threshold)
    {
        switch (greatestAxis)
        {
        case static_cast<int>(Axes::LHorizontal): // Right
            if (axes[greatestAxis] > 0)
            {
                ClientGame::Instance()->SendMovePacket(MoveType::Right);
            }
            else
            {
                ClientGame::Instance()->SendMovePacket(MoveType::Left);
            }
            break;
        case static_cast<int>(Axes::LVertical): // DOWN
            if (axes[greatestAxis] > 0)
            {
                ClientGame::Instance()->SendMovePacket(MoveType::Backward);
            }
            else
            {
                ClientGame::Instance()->SendMovePacket(MoveType::Forward);
            }

            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Handles all button press. Currently does nothing
void ClientGame::HandleButtonPress(const unsigned char* buttons)
{
    using namespace Controller;
    if (buttons[static_cast<int>(Buttons::A)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_A"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_A"), false);
    if (buttons[static_cast<int>(Buttons::B)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_B"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_B"), false);
    if (buttons[static_cast<int>(Buttons::X)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_X"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_X"), false);
    if (buttons[static_cast<int>(Buttons::Y)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Y"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Y"), false);
    if (buttons[static_cast<int>(Buttons::Back)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Back"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Back"), false);
    if (buttons[static_cast<int>(Buttons::Start)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Start"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_Start"), false);
    if (buttons[static_cast<int>(Buttons::LAnalog)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_L_Analog"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_L_Analog"), false);
    if (buttons[static_cast<int>(Buttons::RAnalog)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_R_Analog"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_R_Analog"), false);
    if (buttons[static_cast<int>(Buttons::LBumper)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_L_Bumper"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_L_Bumper"), false);
    if (buttons[static_cast<int>(Buttons::RBumper)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_R_Bumper"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_R_Bumper"), false);
    if (buttons[static_cast<int>(Buttons::DPadUp)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Up"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Up"), false);
    if (buttons[static_cast<int>(Buttons::DPadRight)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Right"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Right"), false);
    if (buttons[static_cast<int>(Buttons::DPadDown)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Down"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Down"), false);
    if (buttons[static_cast<int>(Buttons::DPadLeft)])
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Left"));
    else
        HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("XBOX_D_Pad_Left"), false);
}

void ClientGame::HandleButtonEvent(const std::string& event, bool buttonDown)
{
    if (!event.size())
        return;
    if (buttonDown)
    {
        if (event == EventQuit)
        {
            glfwSetWindowShouldClose(this->m_window, GL_TRUE);
        }
        else if (event == EventWeaponAttack)
        {
            SendAttackPacket(AttackType::Weapon);
        }
        else if (event == EventPeckAttack)
        {
            SendAttackPacket(AttackType::Peck);
        }
        else if (event == EventDiscardWeapon)
        {
            SendDiscardPacket(); // TODOCHANGETHIS
        }
        else if (event == EventJump)
        {
            SendJumpPacket();
        }
        else if (event == EventMoveForward)
        {
            SendMovePacket(MoveType::Forward);
        }
        else if (event == EventMoveBackward)
        {
            SendMovePacket(MoveType::Backward);
        }
        else if (event == EventMoveLeft)
        {
            SendMovePacket(MoveType::Left);
        }
        else if (event == EventMoveRight)
        {
            SendMovePacket(MoveType::Right);
        }
        else if (event == EventScoreboard)
        {
            if (Window::m_pStateManager->GetActiveState()
                == CPlayState::GetInstance(Window::m_pStateManager))
            {
                CPlayState::GetInstance(Window::m_pStateManager)->m_showScoreboard = true;
            }
        }
        else if (event == EventTauntDance)
        {
            printf("dance event triggered\n");
            SendDancePacket();
        }
        else if (event == EventTauntDeath)
        {
            Scene::Instance()->GetPlayer()->TauntDie();
        }
        else if (event == EventTauntPeck)
        {
            Scene::Instance()->GetPlayer()->Peck();
        }
    }
    else // !buttonDown
    {
        if (event == EventScoreboard)
        {
            if (Window::m_pStateManager->GetActiveState()
                == CPlayState::GetInstance(Window::m_pStateManager))
            {
                CPlayState::GetInstance(Window::m_pStateManager)->m_showScoreboard = false;
            }
        }
    }
}
