//
//  Entity.hpp
//  egg scramble
//
//  Created by Phoebe on 4/14/16.
//  Copyright © 2016 sunny side up. All rights reserved.
//
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <SFML/Audio.hpp>

class Shader;
struct DirectionalLight;

enum class PositionKey
{
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
};

class Entity
{
protected:
    glm::mat4 m_toWorld;
    glm::mat3 m_normalMatrix;
    glm::vec3 m_scale;
    GLuint m_VBO, m_VAO, m_EBO;

    std::shared_ptr<Shader> m_shader;

    int m_objId;   // This refers to the specific object
    int m_classId; // This refers to the type of object

    void ApplyScale();
    void CalculateNormalMatrix();
    void LoadDirectionalLight(DirectionalLight* dLight) const;

public:
    ////////////////////////////////////////////////////////////////////////////////
    // NOTE: Constructors do not initialize vertex/element buffers, nor shader
    Entity(int oid, int cid);
    Entity(glm::mat4 world = glm::mat4(1.0f), glm::vec3 scale = glm::vec3(1.0f));
    Entity(glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0f));
    Entity(float x, float y, float z, float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);

    ////////////////////////////////////////////////////////////////////////////////

    // Basic Rendering functions
    virtual void Draw() const = 0;
    virtual void Update(float deltaTime) {}

    // Methods to modify model matrix (position/view)
    virtual void MoveTo(float x, float y, float z) { MoveTo(glm::vec3(x, y, z)); }
    virtual void MoveTo(const glm::vec3& newPosition)
    {
        m_toWorld[3] = glm::vec4(newPosition, 1.0f);
        CalculateNormalMatrix();
    }
    virtual void RotateTo(float w, float x, float y, float z) { RotateTo(glm::quat(w, x, y, z)); }
    virtual void RotateTo(const glm::quat& newOrientation);
    virtual void RotateTo(const glm::mat3& newOrientation);
    void SetScale(const glm::vec3& scale);
    void SetScale(float scale) { SetScale(glm::vec3(scale)); }
    virtual void UseShader() const;
    virtual void SetShaderUniforms() const = 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Getters
    glm::quat Orientation() const;

    const glm::mat4& ToWorld() const { return m_toWorld; }
    const glm::mat3& NormalMatrix() const { return m_normalMatrix; }
    glm::vec3 Position() const { return glm::vec3(m_toWorld[3]); }
    int GetClassId() const { return m_classId; }
    int GetObjId() const { return m_objId; }
    void SetClassId(int cid) { m_classId = cid; }
    void SetObjId(int oid) { m_objId = oid; }
    // Process movement
    void ProcessKeyboard(PositionKey position, GLfloat deltaTime);

    std::shared_ptr<Shader>& GetShader() { return m_shader; }
    // for Players only
    virtual void SetScore(int n) {}
    virtual int GetScore() const { return 0; }
    virtual void SetHealth(int h) {}
    virtual int GetHealth() const { return 0; }
private:
};
