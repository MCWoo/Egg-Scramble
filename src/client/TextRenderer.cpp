// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "TextRenderer.h"
#include "Window.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Shader.h"

std::shared_ptr<Shader> TextRenderer::s_shader;
std::map<GLchar, Character> TextRenderer::s_characters;
GLuint TextRenderer::s_VAO, TextRenderer::s_VBO;

void TextRenderer::Initialize()
{
    // s_shader = Shader("src/Graphics/Shaders/text.vert", "src/Graphics/Shaders/text.frag");
    s_shader = ShaderManager::GetShader("Text");

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<GLfloat>(Window::s_width), static_cast<GLfloat>(Window::s_height), 0.0f);
    s_shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(s_shader->GetProgram(), "projection"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "assets/fonts/AmaticSC-Regular.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {texture,
                               glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                               glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                               (GLuint)face->glyph->advance.x};
        s_characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &s_VAO);
    glGenBuffers(1, &s_VBO);
    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::RenderText(std::string text,
                              GLfloat x,
                              GLfloat y,
                              GLfloat scale,
                              glm::vec3 color)
{
    glDisable(GL_CULL_FACE);
    // Activate corresponding render state
    s_shader->Use();
    glUniform3f(glGetUniformLocation(s_shader->GetProgram(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(s_VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = s_characters[*c];

        GLfloat xpos = x + ch.m_bearing.x * scale;
        GLfloat ypos = y + (s_characters['H'].m_bearing.y - ch.m_bearing.y) * scale;

        GLfloat w = ch.m_size.x * scale;
        GLfloat h = ch.m_size.y * scale;
        // Update s_VBO for each character
        GLfloat vertices[6][4] = {{xpos, ypos + h, 0.0, 1.0},
                                  {xpos + w, ypos, 1.0, 0.0},
                                  {xpos, ypos, 0.0, 0.0},

                                  {xpos, ypos + h, 0.0, 1.0},
                                  {xpos + w, ypos + h, 1.0, 1.0},
                                  {xpos + w, ypos, 1.0, 0.0}};
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.m_textureId);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        sizeof(vertices),
                        vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph
        // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
        x += (ch.m_advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}