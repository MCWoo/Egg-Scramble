#include "InstanceObject.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "client/Window.h"
#include "client/Player.h"
#include "../Scene.h"
#include "../Lights.h"
#include "../Camera.h"
#include "../Model.h"

InstanceObject::InstanceObject(std::shared_ptr<Model> m_instanceModel, GLuint num, GLfloat delta)
    : Entity(), m_instance(m_instanceModel), m_amount(10000)
{
    m_shader = ShaderManager::Instance()->GetShader("Instancing");

    // Generate large list of semi-random transformation matrices
    m_modelMatrices = new glm::mat4[m_amount];
    srand((unsigned int)glfwGetTime()); // initialize random seed
    GLfloat radius = 150.0f;
    GLfloat offset = 25.0f;
    GLfloat size = (float)(sqrt(num) / 2.0f);

    int index = 0;
    for (GLfloat i = -size; i < size; i++)
    {
        for (GLfloat j = -size; j < size; j++)
        {
            glm::mat4 model;
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, i * delta));
            model = glm::translate(model, glm::vec3(j * delta, 0.0f, 0.0f));
            GLfloat scale = ((rand() % 20) / 100.0f + 0.05f) * 5.0f;
            model = glm::scale(model, glm::vec3(scale));
            GLfloat rotAngle = (float)(rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 0.8f, 0.0f));

            m_modelMatrices[index] = model;
            index++;
        }
    }

    for (GLuint i = 0; i < m_instance->Meshes().size(); i++)
    {
        GLuint VAO = m_instance->Meshes()[i].VAO();
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
    delete[] m_modelMatrices;
}

InstanceObject::~InstanceObject()
{
}

void InstanceObject::Draw() const
{
    // Draw the loaded model
    // m_shader->Use();
    UseShader();

    for (GLuint i = 0; i < m_instance->Meshes().size(); i++)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_instance->Textures()[i].m_id);
        glBindVertexArray(m_instance->Meshes()[i].VAO());
        glDrawElementsInstanced(
            GL_TRIANGLES, m_instance->Meshes()[i].m_indices.size(), GL_UNSIGNED_INT, 0, m_amount);
        glBindVertexArray(0);
    }
    if (Scene::Instance()->IsRenderingDepth())
    {
        std::shared_ptr<Shader>& m_shader = Scene::Instance()->GetDepthShader();
        glUniform1i(m_shader->GetUniform("instancing"), GL_FALSE);
    }
}

void InstanceObject::UseShader() const
{
    // For rendering Depth, only need model
    if (Scene::Instance()->IsRenderingDepth())
    {
        std::shared_ptr<Shader>& m_shader = Scene::Instance()->GetDepthShader();
        m_shader->Use();
        glUniformMatrix4fv(m_shader->GetUniform("model"), 1, GL_FALSE, glm::value_ptr(m_toWorld));
        glUniform1i(m_shader->GetUniform("instancing"), GL_TRUE);
    }
    else
    {
        m_shader->Use();
        this->SetShaderUniforms();
    }
}

void InstanceObject::SetShaderUniforms() const
{
    glUniform1i(m_shader->GetUniform("shadowMap"), Scene::Instance()->ShadowMapIndex());
    glActiveTexture(GL_TEXTURE0 + Scene::Instance()->ShadowMapIndex());
    glBindTexture(GL_TEXTURE_2D, Scene::Instance()->DepthMap());

    glUniformMatrix4fv(
        m_shader->GetUniform("view"), 1, false, glm::value_ptr(Scene::Instance()->GetViewMatrix()));
    glUniformMatrix4fv(m_shader->GetUniform("model"), 1, false, glm::value_ptr(m_toWorld));
    glUniformMatrix4fv(m_shader->GetUniform("projection"),
                       1,
                       false,
                       glm::value_ptr(Scene::Instance()->GetPerspectiveMatrix()));
    glUniformMatrix4fv(m_shader->GetUniform("lightSpaceMatrix"),
                       1,
                       false,
                       glm::value_ptr(Scene::Instance()->LightSpaceMatrix()));

    LoadDirectionalLight(Scene::Instance()->GetDirectionalLight());
}
