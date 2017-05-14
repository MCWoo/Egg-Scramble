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

CPlayState::CPlayState(CStateManager* pManager) : CGameState(pManager), m_spriteRenderer(std::make_unique<SpriteRenderer>())
{
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
    m_backgroundMusicID = ClientGame::Instance()->PlaySound("Background", option);
}

void CPlayState::LeaveState()
{
    if (m_backgroundMusicID != -1)
    {
        ClientGame::Instance()->StopSound(m_backgroundMusicID);
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
        glfwSetInputMode(ClientGame::Instance()->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && Window::s_mouseCaptured)
    {
        Window::s_mouseCaptured = false;
        glfwSetInputMode(ClientGame::Instance()->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && Window::s_mouseCaptured)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            ClientGame::Instance()->HandleButtonEvent(
                ConfigManager::Instance()->GetConfigValue("PC_Right_Click"));
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && Window::s_mouseCaptured)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            ClientGame::Instance()->HandleButtonEvent(
                ConfigManager::Instance()->GetConfigValue("PC_Left_Click"));
    }
}

void CPlayState::OnKeyDown(int action, int key)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Esc"));
        break;
    case GLFW_KEY_A:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_A"));
        break;
    case GLFW_KEY_B:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_B"));
        break;
    case GLFW_KEY_C:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_C"));
        break;
    case GLFW_KEY_D:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_D"));
        break;
    case GLFW_KEY_E:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_E"));
        break;
    case GLFW_KEY_F:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_F"));
        break;
    case GLFW_KEY_G:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_G"));
        break;
    case GLFW_KEY_H:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_H"));
        break;
    case GLFW_KEY_I:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_I"));
        break;
    case GLFW_KEY_J:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_J"));
        break;
    case GLFW_KEY_K:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_K"));
        break;
    case GLFW_KEY_L:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_L"));
        break;
    case GLFW_KEY_M:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_M"));
        break;
    case GLFW_KEY_N:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_N"));
        break;
    case GLFW_KEY_O:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_O"));
        break;
    case GLFW_KEY_P:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_P"));
        break;
    case GLFW_KEY_Q:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Q"));
        break;
    case GLFW_KEY_R:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_R"));
        break;
    case GLFW_KEY_S:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_S"));
        break;
    case GLFW_KEY_T:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_T"));
        break;
    case GLFW_KEY_U:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_U"));
        break;
    case GLFW_KEY_V:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_V"));
        break;
    case GLFW_KEY_W:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_W"));
        break;
    case GLFW_KEY_X:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_X"));
        break;
    case GLFW_KEY_Y:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Y"));
        break;
    case GLFW_KEY_Z:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Z"));
        break;
    case GLFW_KEY_0:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_0"));
        break;
    case GLFW_KEY_1:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_1"));
        break;
    case GLFW_KEY_2:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_2"));
        break;
    case GLFW_KEY_3:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_3"));
        break;
    case GLFW_KEY_4:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_4"));
        break;
    case GLFW_KEY_5:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_5"));
        break;
    case GLFW_KEY_6:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_6"));
        break;
    case GLFW_KEY_7:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_7"));
        break;
    case GLFW_KEY_8:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_8"));
        break;
    case GLFW_KEY_9:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_9"));
        break;
    case GLFW_KEY_SPACE:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Space"));
        break;
    case GLFW_KEY_TAB:
        // Check if escape was pressed
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Tab"));
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
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Esc"), false);
        break;
    case GLFW_KEY_A:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_A"),
                                                  false);
        break;
    case GLFW_KEY_B:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_B"),
                                                  false);
        break;
    case GLFW_KEY_C:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_C"),
                                                  false);
        break;
    case GLFW_KEY_D:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_D"),
                                                  false);
        break;
    case GLFW_KEY_E:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_E"),
                                                  false);
        break;
    case GLFW_KEY_F:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_F"),
                                                  false);
        break;
    case GLFW_KEY_G:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_G"),
                                                  false);
        break;
    case GLFW_KEY_H:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_H"),
                                                  false);
        break;
    case GLFW_KEY_I:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_I"),
                                                  false);
        break;
    case GLFW_KEY_J:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_J"),
                                                  false);
        break;
    case GLFW_KEY_K:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_K"),
                                                  false);
        break;
    case GLFW_KEY_L:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_L"),
                                                  false);
        break;
    case GLFW_KEY_M:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_M"),
                                                  false);
        break;
    case GLFW_KEY_N:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_N"),
                                                  false);
        break;
    case GLFW_KEY_O:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_O"),
                                                  false);
        break;
    case GLFW_KEY_P:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_P"),
                                                  false);
        break;
    case GLFW_KEY_Q:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_Q"),
                                                  false);
        break;
    case GLFW_KEY_R:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_R"),
                                                  false);
        break;
    case GLFW_KEY_S:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_S"),
                                                  false);
        break;
    case GLFW_KEY_T:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_T"),
                                                  false);
        break;
    case GLFW_KEY_U:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_U"),
                                                  false);
        break;
    case GLFW_KEY_V:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_V"),
                                                  false);
        break;
    case GLFW_KEY_W:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_W"),
                                                  false);
        break;
    case GLFW_KEY_X:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_X"),
                                                  false);
        break;
    case GLFW_KEY_Y:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_Y"),
                                                  false);
        break;
    case GLFW_KEY_Z:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_Z"),
                                                  false);
        break;
    case GLFW_KEY_0:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_0"),
                                                  false);
        break;
    case GLFW_KEY_1:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_1"),
                                                  false);
        break;
    case GLFW_KEY_2:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_2"),
                                                  false);
        break;
    case GLFW_KEY_3:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_3"),
                                                  false);
        break;
    case GLFW_KEY_4:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_4"),
                                                  false);
        break;
    case GLFW_KEY_5:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_5"),
                                                  false);
        break;
    case GLFW_KEY_6:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_6"),
                                                  false);
        break;
    case GLFW_KEY_7:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_7"),
                                                  false);
        break;
    case GLFW_KEY_8:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_8"),
                                                  false);
        break;
    case GLFW_KEY_9:
        ClientGame::Instance()->HandleButtonEvent(ConfigManager::Instance()->GetConfigValue("PC_9"),
                                                  false);
        break;
    case GLFW_KEY_SPACE:
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Space"), false);
        break;
    case GLFW_KEY_TAB:
        // Check if escape was pressed
        ClientGame::Instance()->HandleButtonEvent(
            ConfigManager::Instance()->GetConfigValue("PC_Tab"), false);
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
        int our_team = ClientGame::Instance()->GetClientTeam();

        ////////////////// BACKGROUND//////////////////////////
        int x = Window::s_width - m_sbChick->Width();
        int y = Window::s_height - m_sbChick->Height();
        m_spriteRenderer->DrawSprite(m_sbChick.get(),
                                    glm::vec2(x, y),
                                    glm::vec2(m_sbChick->Width(), m_sbChick->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        int side_x = 0;
        int side_y = Window::s_height / 2 - m_sbSide->Height() / 2;
        m_spriteRenderer->DrawSprite(m_sbSide.get(),
                                    glm::vec2(side_x, side_y),
                                    glm::vec2(m_sbSide->Width(), m_sbSide->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        glClear(GL_DEPTH_BUFFER_BIT);

        ///////////////// TABLES ////////////////////////////////////////
        x = Texture::GetWindowCenter(m_sbTable->Width());
        y = Window::s_height / 2 - m_sbTable->Height() / 2;
        m_spriteRenderer->DrawSprite(m_sbTable.get(),
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

        for (unsigned int i = 0; i < team0.size(); i++)
        {
            Player* player =
                (Player*)Scene::Instance()->GetEntity(ClassId::Player, team0.at(i)).get();

            std::string score;
            score += ClientGame::Instance()->GetName(team0.at(i)).c_str();
            score += ": ";
            score += std::to_string(player->GetScore());

            TextRenderer::RenderText(score, (float)x0, (float)y0, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));

            y0 = y0 + 80; // row height
        }

        // team 1
        std::vector<int> team1 = ClientGame::Team1();
        for (unsigned int i = 0; i < team1.size(); i++)
        {
            Player* player =
                (Player*)Scene::Instance()->GetEntity(ClassId::Player, team1.at(i)).get();

            std::string score;
            score += ClientGame::Instance()->GetName(team1.at(i)).c_str();
            score += ": ";
            score += std::to_string(player->GetScore());

            TextRenderer::RenderText(score, (float)x1, (float)y1, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));

            y1 = y1 + 80; // row height
        }

        //////////////// TEAM SCORES ///////////////////////////
        printf("our team: %d\n", our_team);
        // us
        TextRenderer::RenderText(
            std::to_string(ClientGame::Instance()->GetScores()[our_team]).c_str(),
            25.0f,
            side_y + 325.0f,
            1.0f,
            glm::vec3(0.0f, 0.0f, 0.0f));

        // them
        TextRenderer::RenderText(
            std::to_string(ClientGame::Instance()->GetScores()[1 - our_team]).c_str(),
            25.0f,
            side_y + 560.0f,
            1.0f,
            glm::vec3(0.0f, 0.0f, 0.0f));

        // neutral
        int n = ClientGame::Instance()->TotalEggs();
        printf("total eggs: %d\n", n);
        printf("score 0: %d\n", ClientGame::Instance()->GetScores()[0]);
        printf("score 1: %d\n", ClientGame::Instance()->GetScores()[1]);
        n = n - ClientGame::Instance()->GetScores()[0] - ClientGame::Instance()->GetScores()[1];
        TextRenderer::RenderText(
            std::to_string(n).c_str(), 25.0f, side_y + 805.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (Scene::Instance()->GetPlayer() != NULL)
    {
        int x = 20;
        int y = Window::s_height - m_hudEgg->Height() - 20;

        //////////////// EGGS COLLECTED ///////////////////////////
        int score = Scene::Instance()->GetPlayer()->GetScore();
        for (int i = 0; i < score; i++)
        {
            m_spriteRenderer->DrawSprite(m_hudEgg.get(),
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
            m_hudWeaponAndTimer.get(),
            glm::vec2(x, y),
            glm::vec2(m_hudWeaponAndTimer->Width(), m_hudWeaponAndTimer->Height()),
            0.0f,
            glm::vec3(1.0f, 1.0f, 1.0f));

        ////////////// Power Up Bar ///////////////////////////////////////
        x = Window::s_width - m_hudPower->Width() - 20;
        y = 20;
        m_spriteRenderer->DrawSprite(m_hudPower.get(),
                                    glm::vec2(x, y),
                                    glm::vec2(m_hudPower->Width(), m_hudPower->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));

        ////////////////////// HEALTH BAR /////////////////////////////////
        x = Texture::GetWindowCenter(m_hudHealth->Width());
        y = 20;

        float health = (float)Scene::Instance()->GetPlayer()->GetHealth();
        std::shared_ptr<Shader>& shader = m_spriteRenderer->GetShader();
        GLint health_pos = shader->GetUniform("health_x_pos");
        glUniform1i(health_pos, (int)(x + (m_hudHealth->Width() * (health / 100.0f))));
        m_spriteRenderer->DrawSprite(m_hudHealth.get(),
                                    glm::vec2(x, y),
                                    glm::vec2(m_hudHealth->Width(), m_hudHealth->Height()),
                                    0.0f,
                                    glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform1i(health_pos, 0);

        switch (static_cast<WeaponType>(Scene::Instance()->GetPlayer()->GetWeapon()))
        {
        case WeaponType::SeedGun:
            m_hudWeapon = m_hudPumpkinSeed.get();
            break;
        case WeaponType::BounceGun:
            m_hudWeapon = m_hudTomato.get();
            break;
        case WeaponType::GrenadeLauncher:
            m_hudWeapon = m_hudPotato.get();
            break;
        case WeaponType::TeleportGun:
            m_hudWeapon = m_hudBB.get();
            break;
        case WeaponType::BlastMine:
            m_hudWeapon = m_hudMine.get();
            break;
        case WeaponType::Shotgun:
            m_hudWeapon = m_hudSeeds.get();
            break;
        default:
            m_hudWeapon = m_weaponMissing.get();
            break;
        }

        x = Window::s_width - m_hudWeaponAndTimer->Width() - 20;
        y = Window::s_height - m_hudWeaponAndTimer->Height() - 20;

        glClear(GL_DEPTH_BUFFER_BIT);
        if (Scene::Instance()->GetPlayer()->GetWeapon() != -1)
        {
            m_spriteRenderer->DrawSprite(m_hudWeapon,
                                        glm::vec2(x, y),
                                        glm::vec2(m_hudWeapon->Width(), m_hudWeapon->Height()),
                                        0.0f,
                                        glm::vec3(1.0f, 1.0f, 1.0f));
        }

        int timer = ClientGame::Instance()->GetCountdown();
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
        TextRenderer::RenderText(time.c_str(), x + 54.0f, y + 190.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        /////////// DEATH OVERLAY ////////////////////////////////////
        if (m_dead)
        { // play loud sound
            glClear(GL_DEPTH_BUFFER_BIT);

            x = Texture::GetWindowCenter(m_deathOverlay->Width());
            y = Window::s_height / 2 - m_deathOverlay->Height() / 2;

            m_spriteRenderer->DrawSprite(m_deathOverlay.get(),
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
        m_sbChick = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/scoreboard_chicken.png");
        m_sbSide = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/m_sbSide_panel.png");
        m_sbTable = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/scoreboard.png");

        m_hudEgg = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/m_hudEgg.png");
        m_hudHealth = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/m_hudHealth.png");
        m_hudPower = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/m_hudPower_up.png");

        m_hudWeaponAndTimer =
            std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/m_hudWeapon&timer.png");

        m_hudTomato = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/tomato_icon.png");
        m_hudPotato = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/potato_icon.png");
        m_hudPumpkinSeed =
            std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/pumpkin_seed_icon.png");
        m_hudBB = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/blueberry_icon.png");
        m_hudMine = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/mine_icon.png");
        m_hudSeeds = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/seeds_icon.png");

        m_weaponMissing = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/weapons/missing_icon.png");

        m_deathOverlay = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/playstate/death.png");

        m_initialized = true;
    }
}
