#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ConfigManager.h"
#include "MenuState.h"
#include "Player.h"
#include "PlayState.h"
#include "StateManager.h"

#include "Graphics/Scene.h"
#include "client/ClientGame.h"
#include "client/TextRenderer.h"

#include <sstream>
#include <math.h>

CPlayState::CPlayState(CStateManager* pManager) : CGameState(pManager), m_spriteRenderer(new SpriteRenderer())
{
}

CPlayState::~CPlayState()
{
    delete m_spriteRenderer;

    delete m_sbChick;
    delete m_sbSide;
    delete m_sbTable;

    delete m_hudEgg;
    delete m_hudHealth;
    delete m_hudPower;

    delete m_hudWeaponAndTimer;
    delete m_hudTomato;
    delete m_hudPotato;
    delete m_hudPumpkinSeed;
    delete m_weaponMissing;

    delete m_deathOverlay;
}

CPlayState* CPlayState::GetInstance(CStateManager* pManager)
{
    static CPlayState Instance(pManager);
    return &Instance;
}

void CPlayState::EnterState()
{
    SoundsHandler::SoundOptions option;
    option.m_isRelativeToListener = true;
    option.m_loops = true;
    m_backgroundMusicID = ClientGame::instance()->PlaySound("Background", option);
}

void CPlayState::LeaveState()
{
    if (m_backgroundMusicID != -1)
    {
        ClientGame::instance()->StopSound(m_backgroundMusicID);
        m_backgroundMusicID = -1;
    }
}

void CPlayState::Reset()
{
    m_dead = false;
    m_showScoreboard = false;
}

void CPlayState::OnMouseMove(float xoffset, float yoffset)
{
    Player* player = Scene::Instance()->GetPlayer();
    if (player)
    {
        player->ProcessMouseMovement(xoffset, yoffset);
    }
}

void CPlayState::OnClick(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && !Window::s_mouseCaptured)
    {
        Window::s_mouseCaptured = true;
        Window::s_firstMouse = true;
        glfwSetInputMode(ClientGame::instance()->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && Window::s_mouseCaptured)
    {
        Window::s_mouseCaptured = false;
        glfwSetInputMode(ClientGame::instance()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && Window::s_mouseCaptured)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            ClientGame::instance()->HandleButtonEvent(
                ConfigManager::instance()->GetConfigValue("PC_Right_Click"));
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && Window::s_mouseCaptured)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            ClientGame::instance()->HandleButtonEvent(
                ConfigManager::instance()->GetConfigValue("PC_Left_Click"));
    }
}

void CPlayState::OnKeyDown(int action, int key)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Esc"));
        break;
    case GLFW_KEY_A:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_A"));
        break;
    case GLFW_KEY_B:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_B"));
        break;
    case GLFW_KEY_C:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_C"));
        break;
    case GLFW_KEY_D:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_D"));
        break;
    case GLFW_KEY_E:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_E"));
        break;
    case GLFW_KEY_F:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_F"));
        break;
    case GLFW_KEY_G:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_G"));
        break;
    case GLFW_KEY_H:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_H"));
        break;
    case GLFW_KEY_I:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_I"));
        break;
    case GLFW_KEY_J:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_J"));
        break;
    case GLFW_KEY_K:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_K"));
        break;
    case GLFW_KEY_L:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_L"));
        break;
    case GLFW_KEY_M:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_M"));
        break;
    case GLFW_KEY_N:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_N"));
        break;
    case GLFW_KEY_O:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_O"));
        break;
    case GLFW_KEY_P:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_P"));
        break;
    case GLFW_KEY_Q:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Q"));
        break;
    case GLFW_KEY_R:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_R"));
        break;
    case GLFW_KEY_S:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_S"));
        break;
    case GLFW_KEY_T:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_T"));
        break;
    case GLFW_KEY_U:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_U"));
        break;
    case GLFW_KEY_V:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_V"));
        break;
    case GLFW_KEY_W:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_W"));
        break;
    case GLFW_KEY_X:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_X"));
        break;
    case GLFW_KEY_Y:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Y"));
        break;
    case GLFW_KEY_Z:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Z"));
        break;
    case GLFW_KEY_0:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_0"));
        break;
    case GLFW_KEY_1:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_1"));
        break;
    case GLFW_KEY_2:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_2"));
        break;
    case GLFW_KEY_3:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_3"));
        break;
    case GLFW_KEY_4:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_4"));
        break;
    case GLFW_KEY_5:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_5"));
        break;
    case GLFW_KEY_6:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_6"));
        break;
    case GLFW_KEY_7:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_7"));
        break;
    case GLFW_KEY_8:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_8"));
        break;
    case GLFW_KEY_9:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_9"));
        break;
    case GLFW_KEY_SPACE:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Space"));
        break;
    case GLFW_KEY_TAB:
        // Check if escape was pressed
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Tab"));
        break;
    default:
        break;
    }
}

void CPlayState::OnKeyUp(int action, int key)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Esc"), false);
        break;
    case GLFW_KEY_A:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_A"),
                                                  false);
        break;
    case GLFW_KEY_B:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_B"),
                                                  false);
        break;
    case GLFW_KEY_C:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_C"),
                                                  false);
        break;
    case GLFW_KEY_D:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_D"),
                                                  false);
        break;
    case GLFW_KEY_E:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_E"),
                                                  false);
        break;
    case GLFW_KEY_F:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_F"),
                                                  false);
        break;
    case GLFW_KEY_G:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_G"),
                                                  false);
        break;
    case GLFW_KEY_H:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_H"),
                                                  false);
        break;
    case GLFW_KEY_I:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_I"),
                                                  false);
        break;
    case GLFW_KEY_J:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_J"),
                                                  false);
        break;
    case GLFW_KEY_K:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_K"),
                                                  false);
        break;
    case GLFW_KEY_L:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_L"),
                                                  false);
        break;
    case GLFW_KEY_M:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_M"),
                                                  false);
        break;
    case GLFW_KEY_N:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_N"),
                                                  false);
        break;
    case GLFW_KEY_O:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_O"),
                                                  false);
        break;
    case GLFW_KEY_P:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_P"),
                                                  false);
        break;
    case GLFW_KEY_Q:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_Q"),
                                                  false);
        break;
    case GLFW_KEY_R:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_R"),
                                                  false);
        break;
    case GLFW_KEY_S:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_S"),
                                                  false);
        break;
    case GLFW_KEY_T:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_T"),
                                                  false);
        break;
    case GLFW_KEY_U:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_U"),
                                                  false);
        break;
    case GLFW_KEY_V:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_V"),
                                                  false);
        break;
    case GLFW_KEY_W:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_W"),
                                                  false);
        break;
    case GLFW_KEY_X:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_X"),
                                                  false);
        break;
    case GLFW_KEY_Y:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_Y"),
                                                  false);
        break;
    case GLFW_KEY_Z:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_Z"),
                                                  false);
        break;
    case GLFW_KEY_0:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_0"),
                                                  false);
        break;
    case GLFW_KEY_1:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_1"),
                                                  false);
        break;
    case GLFW_KEY_2:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_2"),
                                                  false);
        break;
    case GLFW_KEY_3:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_3"),
                                                  false);
        break;
    case GLFW_KEY_4:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_4"),
                                                  false);
        break;
    case GLFW_KEY_5:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_5"),
                                                  false);
        break;
    case GLFW_KEY_6:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_6"),
                                                  false);
        break;
    case GLFW_KEY_7:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_7"),
                                                  false);
        break;
    case GLFW_KEY_8:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_8"),
                                                  false);
        break;
    case GLFW_KEY_9:
        ClientGame::instance()->HandleButtonEvent(ConfigManager::instance()->GetConfigValue("PC_9"),
                                                  false);
        break;
    case GLFW_KEY_SPACE:
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Space"), false);
        break;
    case GLFW_KEY_TAB:
        // Check if escape was pressed
        ClientGame::instance()->HandleButtonEvent(
            ConfigManager::instance()->GetConfigValue("PC_Tab"), false);
        break;
    default:
        break;
    }
}

void CPlayState::Update(DWORD dwCurrentTime)
{
    // update scene
    Scene::Instance()->Update();
}

void CPlayState::Draw()
{
    InitTextures();

    Scene::Instance()->Draw();

    //////////////// SCOREBOARD ///////////////////////////////////
    if (m_showScoreboard)
    {
        int our_team = ClientGame::instance()->GetClientTeam();

        ////////////////// BACKGROUND//////////////////////////
        int x = Window::s_width - m_sbChick->Width();
        int y = Window::s_height - m_sbChick->Height();
        m_spriteRenderer->DrawSprite(*m_sbChick,
                                    glm::vec2(x, y),
                                    glm::vec2(m_sbChick->Width(), m_sbChick->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        int side_x = 0;
        int side_y = Window::s_height / 2 - m_sbSide->Height() / 2;
        m_spriteRenderer->DrawSprite(*m_sbSide,
                                    glm::vec2(side_x, side_y),
                                    glm::vec2(m_sbSide->Width(), m_sbSide->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        glClear(GL_DEPTH_BUFFER_BIT);

        ///////////////// TABLES ////////////////////////////////////////
        x = Texture::GetWindowCenter(m_sbTable->Width());
        y = Window::s_height / 2 - m_sbTable->Height() / 2;
        m_spriteRenderer->DrawSprite(*m_sbTable,
                                    glm::vec2(x, y),
                                    glm::vec2(m_sbTable->Width(), m_sbTable->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        glClear(GL_DEPTH_BUFFER_BIT);

        // team 0
        std::vector<int> team0 = ClientGame::Team0();

        int y0 = y + 140, y1 = y0;
        int x0, x1;
        if (our_team == 0)
        {
            x0 = x + 445; // our team displays on right
            x1 = x + 45;
        }
        else
        {
            x0 = x + 45;
            x1 = x + 445;
        }

        for (int i = 0; i < team0.size(); i++)
        {
            Player* player =
                (Player*)Scene::Instance()->GetEntity(ClassId::Player, team0.at(i)).get();

            std::string score;
            score += ClientGame::instance()->GetName(team0.at(i)).c_str();
            score += ": ";
            score += std::to_string(player->GetScore());

            TextRenderer::RenderText(score, x0, y0, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));

            y0 = y0 + 80; // row height
        }

        // team 1
        std::vector<int> team1 = ClientGame::Team1();
        for (int i = 0; i < team1.size(); i++)
        {
            Player* player =
                (Player*)Scene::Instance()->GetEntity(ClassId::Player, team1.at(i)).get();

            std::string score;
            score += ClientGame::instance()->GetName(team1.at(i)).c_str();
            score += ": ";
            score += std::to_string(player->GetScore());

            TextRenderer::RenderText(score, x1, y1, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));

            y1 = y1 + 80; // row height
        }

        //////////////// TEAM SCORES ///////////////////////////
        printf("our team: %d\n", our_team);
        // us
        TextRenderer::RenderText(
            std::to_string(ClientGame::instance()->GetScores()[our_team]).c_str(),
            25,
            side_y + 325,
            1.0f,
            glm::vec3(0.0f, 0.0f, 0.0f));

        // them
        TextRenderer::RenderText(
            std::to_string(ClientGame::instance()->GetScores()[1 - our_team]).c_str(),
            25,
            side_y + 560,
            1.0f,
            glm::vec3(0.0f, 0.0f, 0.0f));

        // neutral
        int n = ClientGame::instance()->TotalEggs();
        printf("total eggs: %d\n", n);
        printf("score 0: %d\n", ClientGame::instance()->GetScores()[0]);
        printf("score 1: %d\n", ClientGame::instance()->GetScores()[1]);
        n = n - ClientGame::instance()->GetScores()[0] - ClientGame::instance()->GetScores()[1];
        TextRenderer::RenderText(
            std::to_string(n).c_str(), 25, side_y + 805, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (Scene::Instance()->GetPlayer() != NULL)
    {
        int x = 20;
        int y = Window::s_height - m_hudEgg->Height() - 20;

        //////////////// EGGS COLLECTED ///////////////////////////
        int score = Scene::Instance()->GetPlayer()->GetScore();
        for (int i = 0; i < score; i++)
        {
            m_spriteRenderer->DrawSprite(*m_hudEgg,
                                        glm::vec2(x, y),
                                        glm::vec2(m_hudEgg->Width(), m_hudEgg->Height()),
                                        0.0f,
                                        glm::vec3(1.0f, 1.0f, 1.0f));
            x = x + m_hudEgg->Width() + 5;
        }

        ////////////// WEAPON & TIMER ///////////////////////////////////
        x = Window::s_width - m_hudWeaponAndTimer->Width() - 20;
        y = Window::s_height - m_hudWeaponAndTimer->Height() - 20;
        m_spriteRenderer->DrawSprite(
            *m_hudWeaponAndTimer,
            glm::vec2(x, y),
            glm::vec2(m_hudWeaponAndTimer->Width(), m_hudWeaponAndTimer->Height()),
            0.0f,
            glm::vec3(1.0f, 1.0f, 1.0f));

        ////////////// Power Up Bar ///////////////////////////////////////
        x = Window::s_width - m_hudPower->Width() - 20;
        y = 20;
        m_spriteRenderer->DrawSprite(*m_hudPower,
                                    glm::vec2(x, y),
                                    glm::vec2(m_hudPower->Width(), m_hudPower->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        ////////////////////// HEALTH BAR /////////////////////////////////
        x = Texture::GetWindowCenter(m_hudHealth->Width());
        y = 20;

        float health = Scene::Instance()->GetPlayer()->GetHealth();
        std::shared_ptr<Shader>& shader = m_spriteRenderer->GetShader();
        GLint health_pos = shader->GetUniform("health_x_pos");
        glUniform1i(health_pos, x + (m_hudHealth->Width() * (health / 100.0f)));
        m_spriteRenderer->DrawSprite(*m_hudHealth,
                                    glm::vec2(x, y),
                                    glm::vec2(m_hudHealth->Width(), m_hudHealth->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform1i(health_pos, 0);

        switch (static_cast<WeaponType>(Scene::Instance()->GetPlayer()->GetWeapon()))
        {
        case WeaponType::SeedGun:
            m_hudWeapon = m_hudPumpkinSeed;
            break;
        case WeaponType::BounceGun:
            m_hudWeapon = m_hudTomato;
            break;
        case WeaponType::GrenadeLauncher:
            m_hudWeapon = m_hudPotato;
            break;
        case WeaponType::TeleportGun:
            m_hudWeapon = m_hudBB;
            break;
        case WeaponType::BlastMine:
            m_hudWeapon = m_hudMine;
            break;
        case WeaponType::Shotgun:
            m_hudWeapon = m_hudSeeds;
            break;
        default:
            m_hudWeapon = m_weaponMissing;
            break;
        }

        x = Window::s_width - m_hudWeaponAndTimer->Width() - 20;
        y = Window::s_height - m_hudWeaponAndTimer->Height() - 20;

        glClear(GL_DEPTH_BUFFER_BIT);
        if (Scene::Instance()->GetPlayer()->GetWeapon() != -1)
        {
            m_spriteRenderer->DrawSprite(*m_hudWeapon,
                                        glm::vec2(x, y),
                                        glm::vec2(m_hudWeapon->Width(), m_hudWeapon->Height()),
                                        0.0f,
                                        glm::vec3(1.0f, 1.0f, 1.0f));
        }

        int timer = ClientGame::instance()->GetCountdown();
        int minutes = timer / 60;
        int seconds = timer % 60;
        std::string time;
        if (timer == 0)
        {
            time += "5";
            time += ":";
            time += "00";
        }
        else
        {
            time += std::to_string(minutes);
            time += ":";
            if (seconds == 0)
                time += "00";
            else if (seconds < 10)
            {
                time += "0";
                time += std::to_string(seconds);
            }
            else
                time += std::to_string(seconds);
        }
        glClear(GL_DEPTH_BUFFER_BIT);
        TextRenderer::RenderText(time.c_str(), x + 54, y + 190, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        /////////// DEATH OVERLAY ////////////////////////////////////
        if (m_dead)
        { // play loud sound
            glClear(GL_DEPTH_BUFFER_BIT);

            x = Texture::GetWindowCenter(m_deathOverlay->Width());
            y = Window::s_height / 2 - m_deathOverlay->Height() / 2;

            m_spriteRenderer->DrawSprite(*m_deathOverlay,
                                        glm::vec2(x, y),
                                        glm::vec2(m_deathOverlay->Width(), m_deathOverlay->Height()),
                                        0.0f,
                                        glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void CPlayState::InitTextures()
{
    if (!m_initialized)
    {
        // Create the different images
        m_sbChick = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/scoreboard_chicken.png");
        m_sbSide = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/m_sbSide_panel.png");
        m_sbTable = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/scoreboard.png");

        m_hudEgg = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/m_hudEgg.png");
        m_hudHealth = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/m_hudHealth.png");
        m_hudPower = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/m_hudPower_up.png");

        m_hudWeaponAndTimer =
            new Texture(GL_TEXTURE_2D, "assets/ui/playstate/m_hudWeapon&timer.png");

        m_hudTomato = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/tomato_icon.png");
        m_hudPotato = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/potato_icon.png");
        m_hudPumpkinSeed =
            new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/pumpkin_seed_icon.png");
        m_hudBB = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/blueberry_icon.png");
        m_hudMine = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/mine_icon.png");
        m_hudSeeds = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/seeds_icon.png");

        m_weaponMissing = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/weapons/missing_icon.png");

        m_deathOverlay = new Texture(GL_TEXTURE_2D, "assets/ui/playstate/death.png");

        m_initialized = true;
    }
}
