#include "MenuState.h"

#include "LobbyState.h"
#include "StateManager.h"
#include "TextRenderer.h"
#include "Window.h"

#include "Graphics/Shader.h"
#include "Graphics/Scene.h"
#include "ClientGame.h"

CMenuState::CMenuState(CStateManager* manager)
    : CGameState(manager), m_spriteRenderer(std::make_unique<SpriteRenderer>())
{
}

CMenuState* CMenuState::GetInstance(CStateManager* manager)
{
    static CMenuState Instance(manager);
    return &Instance;
}

void CMenuState::OnKeyDown(int action, int key)
{
    if (m_typing)
    {
        static bool first = true;
        if (first)
        {
            m_username.clear();
            first = false;
        }
        if (action == GLFW_REPEAT && key == GLFW_KEY_BACKSPACE)
        { // erase name
            m_username.clear();
        }
        else if (key == GLFW_KEY_BACKSPACE && m_username.size())
        {
            m_username.pop_back(); // remove last char
        }
        else if (key == GLFW_KEY_ENTER)
        {
            StartGame();
        }
    }
}

void CMenuState::OnClick(int button, int action, double x, double y)
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
            m_typing = false;
            break;
        case 1:
            printf("Textbox clicked\n");
            if (m_username == m_defaultName)
            {
                ClientGame::Instance()->PlaySound("Button_Click", options);
                m_username.clear();
            }
            m_typing = true;
            break;
        case 2:
            printf("Button clicked\n");
            ClientGame::Instance()->PlaySound("Button_Click", options);
            StartGame();
            break;
        default:
            //printf("%d clicked%s\n", res[0]);
            printf("%d clicked\n", res[0]); // Is this the right format string??
            ClientGame::Instance()->PlaySound("Button_Click", options);
            m_typing = false;
        }
    }
}

void CMenuState::OnChar(unsigned int codepoint)
{
    if (m_typing)
    {
        m_username += (char)codepoint;
        std::cout << m_username.c_str() << std::endl;
    }
}

void CMenuState::RenderSelection()
{
    InitTextures();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ////////////////// BACKGROUND//////////////////////////
    float x = (float)Texture::GetWindowCenter(m_bg->Width());
    float y = (float)Window::s_height / 2 - m_bg->Height() / 2;

    ////////////// USERNAME TEXTBOX /////////////////////////////////
    m_spriteRenderer->RenderSelection(1,
                                     m_textbox.get(),
                                     glm::vec2(x + 115, y + 555),
                                     glm::vec2(m_textbox->Width(), m_textbox->Height()),
                                     0.0f);

    ////////////// JOIN BUTTON /////////////////////////////////////
    m_spriteRenderer->RenderSelection(2,
                                      m_join.get(),
                                      glm::vec2(x + 660, y + 555),
                                      glm::vec2(m_join->Width(), m_join->Height()),
                                      0.0f);

    glClearColor(0.28f, 0.65f, 0.89f, 1.0f); // reset color
}

void CMenuState::Update(DWORD)
{
    ClientGame::Instance()->SetName(m_username);
}

void CMenuState::Draw()
{
    InitTextures();

    ////////////////// BACKGROUND//////////////////////////
    float x = (float)Texture::GetWindowCenter(m_bg->Width());
    float y = (float)Window::s_height / 2 - m_bg->Height() / 2;
    m_spriteRenderer->DrawSprite(m_bg.get(),
                                glm::vec2(x, y),
                                glm::vec2(m_bg->Width(), m_bg->Height()),
                                0.0f,
                                glm::vec3(1.0f, 1.0f, 1.0f));

    ////////////// USERNAME TEXTBOX /////////////////////////////////
    m_spriteRenderer->DrawSprite(m_textbox.get(),
                                glm::vec2(x + 115, y + 555),
                                glm::vec2(m_textbox->Width(), m_textbox->Height()),
                                0.0f,
                                glm::vec3(1.0f, 1.0f, 1.0f));

    TextRenderer::RenderText(m_username.c_str(), x + 145, y + 585, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));

    ////////////// JOIN BUTTON /////////////////////////////////////
    m_spriteRenderer->DrawSprite(m_join.get(),
                                glm::vec2(x + 660, y + 555),
                                glm::vec2(m_join->Width(), m_join->Height()),
                                0.0f,
                                glm::vec3(1.0f, 1.0f, 1.0f));
}

void CMenuState::EnterState()
{
    m_defaultName += "Enter your name";

    m_username += m_defaultName;
    m_typing = true;
    // ClientGame::Instance()->PlayMenuSound();      // DO NOT CALL HERE. Loops
}

void ::CMenuState::InitTextures()
{
    if (!m_initialized)
    {
        // Create the different images
        m_bg = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/login/login_bg.png");
        m_textbox = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/login/login_textbox.png");
        m_join = std::make_unique<Texture>(GL_TEXTURE_2D, "assets/ui/login/login_button.png");

        m_initialized = true;
    }
}

void CMenuState::StartGame()
{
    // send chicken name
    ClientGame::Instance()->SendNamePacket();
    // change state
    m_stateManager->ChangeState(LobbyState::GetInstance(m_stateManager));
}
