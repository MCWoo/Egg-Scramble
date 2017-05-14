#include "LobbyState.h"
#include "PlayState.h"
#include "StateManager.h"
#include "TextRenderer.h"
#include "Window.h"

#include "Graphics/Shader.h"
#include "ClientGame.h"

#include <string.h>

LobbyState::LobbyState(CStateManager* pManager)
    : CGameState(pManager), m_spriteRenderer(std::make_unique<SpriteRenderer>())
{
    // Create the text controls of the menu.
    ClientGame::Instance()->SendJoinPacket(ClientGame::GetClientId() % 2);
}

LobbyState* LobbyState::GetInstance(CStateManager* pManager)
{
    static LobbyState Instance(pManager);
    return &Instance;
}

void LobbyState::OnClick(int button, int action, double x, double y)
{
    if (!m_loading)
    {
        GLubyte res[4];
        GLint viewport[4];

        // render selection
        RenderSelection();

        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels((int)x, (int)(viewport[3] - y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
        if (action == GLFW_PRESS)
        {
            SoundsHandler::SoundOptions options;
            options.m_isRelativeToListener = true;
            switch (res[0])
            {
            case 0:
                printf("None clicked\n");
                break;
            case 1:
                printf("Start Button clicked\n");
                ClientGame::Instance()->PlaySound("Button_Click", options);
                ClientGame::Instance()->SendStartPacket();
                break;
            case 2:
                printf("Join T0 clicked\n"); // team 1
                ClientGame::Instance()->PlaySound("Button_Click", options);
                ClientGame::Instance()->SendJoinPacket(0);
                break;
            case 3:
                printf("Join T1 clicked\n"); // team 2
                ClientGame::Instance()->PlaySound("Button_Click", options);
                ClientGame::Instance()->SendJoinPacket(1);
                break;
            default:
                //printf("%d clicked%s\n", res[0]);
                printf("%d clicked\n", res[0]); // Is this the right format string??
                ClientGame::Instance()->PlaySound("Button_Click", options);
            }
        }
    }
}

void LobbyState::RenderSelection()
{
    if (!m_loading)
    {
        InitTextures();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ////////////////// BACKGROUND//////////////////////////
        float x = (float)Texture::GetWindowCenter(m_bg->Width());
        float y = Window::s_height / 2.0f - m_bg->Height() / 2.0f;

        ////////////// START BUTTON /////////////////////////////////////
        m_spriteRenderer->RenderSelection(
            1,
            m_startButton.get(),
            glm::vec2(x + 1000, y + 55),
            glm::vec2(m_startButton->Width(), m_startButton->Height()),
            0.0f);

        //////////////// JOIN BUTTONS ////////////////////////////////
        float ty = y + 255;
        float tx1 = x + (m_bg->Width() / 2) - (m_tableT1->Width() + 20);
        float tx2 = tx1 + m_tableT1->Width() + 40;

        // adjust tx, ty for transparent pixels
        tx1 += 4;
        tx2 += 4;
        ty += 5;

        std::vector<int> team0 = ClientGame::Team0();
        if (std::find(team0.begin(), team0.end(), ClientGame::GetClientId()) == team0.end())
        {
            // add m_join button for t1
            m_spriteRenderer->RenderSelection(2,
                                              m_join.get(),
                                              glm::vec2(tx1, ty + 390),
                                              glm::vec2(m_join->Width(), m_join->Height()),
                                              0.0f);
        }
        else
        { // t2
            m_spriteRenderer->RenderSelection(3,
                                              m_join.get(),
                                              glm::vec2(tx2, ty + 390),
                                              glm::vec2(m_join->Width(), m_join->Height()),
                                              0.0f);
        }

        glClearColor(0.28f, 0.65f, 0.89f, 1.0f); // reset color
    }
}

void LobbyState::Draw()
{
    if (!m_loading)
    {
        InitTextures();

        ////////////////// BACKGROUND//////////////////////////
        float x = (float)Texture::GetWindowCenter(m_bg->Width());
        float y = Window::s_height / 2.0f - m_bg->Height() / 2.0f;
        m_spriteRenderer->DrawSprite(m_bg.get(),
                                     glm::vec2(x, y),
                                     glm::vec2(m_bg->Width(), m_bg->Height()),
                                     0.0f,
                                     glm::vec3(1.0f, 1.0f, 1.0f));

        ////////////// START BUTTON /////////////////////////////////////
        m_spriteRenderer->DrawSprite(m_startButton.get(),
                                     glm::vec2(x + 1000, y + 55),
                                     glm::vec2(m_startButton->Width(), m_startButton->Height()),
                                     0.0f,
                                     glm::vec3(1.0f, 1.0f, 1.0f));

        ///////////////// TABLES ////////////////////////////////////////
        float ty = y + 255;

        float tx1 = x + (m_bg->Width() / 2) - (m_tableT1->Width() + 20);
        m_spriteRenderer->DrawSprite(m_tableT1.get(),
                                     glm::vec2(tx1, ty),
                                     glm::vec2(m_tableT1->Width(), m_tableT1->Height()),
                                     0.0f,
                                     glm::vec3(1.0f, 1.0f, 1.0f));

        float tx2 = tx1 + m_tableT1->Width() + 40;
        m_spriteRenderer->DrawSprite(m_tableT2.get(),
                                     glm::vec2(tx2, ty),
                                     glm::vec2(m_tableT2->Width(), m_tableT2->Height()),
                                     0.0f,
                                     glm::vec3(1.0f, 1.0f, 1.0f));

        // adjust tx, ty for transparent pixels
        tx1 += 4;
        tx2 += 4;
        ty += 5;

        ///////////// TEAM 0 //////////////////////////////////////////////
        std::vector<int> team0 = ClientGame::Team0();
        x = tx1 + 6;
        y = ty + 90 + 6; // skip header

        for (unsigned int i = 0; i < team0.size(); i++)
        {
            TextRenderer::RenderText(ClientGame::Instance()->GetName(team0.at(i)).c_str(),
                                     x,
                                     y,
                                     1.0f,
                                     glm::vec3(0.5f, 0.5f, 0.5f));

            y = y + 60; // row height
        }

        if (std::find(team0.begin(), team0.end(), ClientGame::GetClientId()) == team0.end())
        {
            // add m_join button
            m_spriteRenderer->DrawSprite(m_join.get(),
                                         glm::vec2(tx1, ty + 390),
                                         glm::vec2(m_join->Width(), m_join->Height()),
                                         0.0f,
                                         glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else
        {
            m_spriteRenderer->DrawSprite(m_joinDisabled.get(),
                                         glm::vec2(tx1, ty + 390),
                                         glm::vec2(m_join->Width(), m_join->Height()),
                                         0.0f,
                                         glm::vec3(1.0f, 1.0f, 1.0f));
        }

        ///////////// TEAM 1 //////////////////////////////////////////////
        std::vector<int> team1 = ClientGame::Team1();
        x = tx2 + 6;
        y = ty + 90 + 6; // skip header

        for (unsigned int i = 0; i < team1.size(); i++)
        {
            TextRenderer::RenderText(ClientGame::Instance()->GetName(team1.at(i)).c_str(),
                                     x,
                                     y,
                                     1.0f,
                                     glm::vec3(0.5f, 0.5f, 0.5f));

            y = y + 60; // row height
        }

        if (std::find(team1.begin(), team1.end(), ClientGame::GetClientId()) == team1.end())
        {
            // add m_join button
            m_spriteRenderer->DrawSprite(m_join.get(),
                                         glm::vec2(tx2, ty + 390),
                                         glm::vec2(m_join->Width(), m_join->Height()),
                                         0.0f,
                                         glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else
        {
            m_spriteRenderer->DrawSprite(m_joinDisabled.get(),
                                         glm::vec2(tx2, ty + 390),
                                         glm::vec2(m_join->Width(), m_join->Height()),
                                         0.0f,
                                         glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
    else
    {
        ShowLoadingScreen();
    }
}

void LobbyState::EnterState()
{
}

void LobbyState::LeaveState()
{
    ClientGame::Instance()->StopMenuSound();
}

void LobbyState::InitTextures()
{
    if (!m_initialized)
    {
        // Create the different images
        m_bg = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_bg.png");
        m_startButton = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_start.png");

        m_tableT1 = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_t1.png");
        m_tableT2 = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_t2.png");

        m_join = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_join.png");
        m_joinDisabled = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/lobby/lobby_join_disabled.png");

        m_loadScreen = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/loading/instructions.png");
        m_initialized = true;
    }
}

void LobbyState::ShowLoadingScreen()
{
    int x = Texture::GetWindowCenter(m_loadScreen->Width());
    int y = Window::s_height / 2 - m_loadScreen->Height() / 2;

    m_spriteRenderer->DrawSprite(m_loadScreen.get(),
                                 glm::vec2(x, y),
                                 glm::vec2(m_loadScreen->Width(), m_loadScreen->Height()),
                                 0.0f,
                                 glm::vec3(1.0f, 1.0f, 1.0f));

    // glfwSwapBuffers(ClientGame::Instance()->window);
}
