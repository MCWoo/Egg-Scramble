#include "Grass.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "client/Window.h"
#include "client/Player.h"
#include "../Scene.h"
#include "../Lights.h"
#include "../Camera.h"
#include "../Model.h"
#include "../ModelManager.h"

Grass::Grass(const GLchar* path, GLuint num) : Entity(), m_amount(num)
{
    m_grass = ModelManager::GetModel("Grass");
    m_shader = ShaderManager::Instance()->GetShader("Instancing");

    // Generate large list of semi-random transformation matrices
    m_amount = 10000;
    m_modelMatrices = new glm::mat4[m_amount];
    srand((unsigned int)glfwGetTime()); // initialize random seed
    GLfloat radius = 150.0f;
    GLfloat offset = 25.0f;
    int index = 0;
    for (GLfloat i = -50.0f; i < 50.0f; i++)
    {
        for (GLfloat j = -50.0f; j < 50.0f; j++)
        {
            glm::mat4 model;
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, i));
            model = glm::translate(model, glm::vec3(j, 0.0f, 0.0f));
            GLfloat scale = (float)((rand() % 20) / 100.0f + 0.05) * 5.0f;
            model = glm::scale(model, glm::vec3(scale));

            GLfloat rotAngle = (float)(rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 0.8f, 0.0f));

            m_modelMatrices[index] = model;
            index++;
        }
    }

    for (GLuint i = 0; i < m_grass->Meshes().size(); i++)
    {
        GLuint VAO = m_grass->Meshes()[i].VAO();
        GLuint buffer;
        glBindVertexArray(VAO);
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(
            GL_ARRAY_BUFFER, m_amount * sizeof(glm::mat4), &m_modelMatrices[0], GL_STATIC_DRAW);
        // Set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(
            5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(
            6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

Grass::~Grass()
{
    delete[] m_modelMatrices;
}

void Grass::Draw() const
{
    // Set frame time
    // GLfloat currentFrame = glfwGetTime();
    // deltaTime = currentFrame - lastFrame;
    // lastFrame = currentFrame;

    // Use the appropriate m_shader (depth or model)
    UseShader();

    for (GLuint i = 0; i < m_grass->Meshes().size(); i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_grass->Textures()[i].m_id);
        glBindVertexArray(m_grass->Meshes()[i].VAO());
        glDrawElementsInstanced(
            GL_TRIANGLES, m_grass->Meshes()[i].m_vertices.size(), GL_UNSIGNED_INT, 0, m_amount);
        glBindVertexArray(0);
    }

    // model->Draw(m_shader.get());
}

void Grass::Update(float deltaTime)
{
}

void Grass::SetShaderUniforms() const
{
    glUniformMatrix4fv(
        m_shader->GetUniform("view"), 1, false, glm::value_ptr(Scene::Instance()->GetViewMatrix()));
    glUniformMatrix4fv(m_shader->GetUniform("model"), 1, false, glm::value_ptr(m_toWorld));
    glUniformMatrix4fv(m_shader->GetUniform("projection"),
                       1,
                       false,
                       glm::value_ptr(Scene::Instance()->GetPerspectiveMatrix()));
    LoadDirectionalLight(Scene::Instance()->GetDirectionalLight());
}
