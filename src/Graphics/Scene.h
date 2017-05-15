#pragma once
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>

#include "Objects/Entity.h"
#include "Objects/Ground.h"
#include "Objects/Grass.h"
#include "ShaderManager.h"
#include "client/SpriteRenderer.h"

#include "server/engine/ObjectId.h"

class Camera;
class CubeMap;
class InstanceObject;
class Player;
struct PosInfo;
class Shader;
class StaticObject;

struct PointLight;
struct DirectionalLight;

class Scene : public Singleton<Scene>
{
    friend class Singleton<Scene>;

    enum class UboIndex
    {
        Matrices = 0,
        Lights = 1,
    };

    // const GLuint shadowDepthWidth = 4096, shadowDepthHeight = 4096;
    const GLuint shadowDepthWidth = 1024, shadowDepthHeight = 1024;
    unsigned int m_shadowMapIndex = 7;
    float m_lastTime;
    GLuint m_uboMatricesBuffer;
    GLuint m_depthMapFbo;
    GLuint m_depthMap;
    bool m_renderingDepthMap = false;
    std::shared_ptr<Shader> m_depthShader;
    glm::mat4 m_lightSpaceMatrix;

    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<PointLight> m_pointLight;
    std::unique_ptr<DirectionalLight> m_dirLight;
    std::unique_ptr<CubeMap> m_cubeMap;
    std::unique_ptr<Ground> m_ground;
    std::unique_ptr<InstanceObject> m_grass;
    std::unique_ptr<InstanceObject> m_pumpkin;

    Player* player = nullptr;

    static const int s_width;
    static const int s_height;

    std::map<std::pair<ClassId, int>, std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<StaticObject>> m_staticObjects;

    Scene();
    void Setup();
    void InitializeUbos();
    void InitializeFbo();
    void SetViewUbo();
    void SetLightSpaceUbo();
    void SetProjectionUbo();
    void ConfigureShaderAndMatrices();
    void RenderDepthMap();
    void DrawDepthMap();
    void RenderScene();
    void RenderQuad();

public:
    static SpriteRenderer* sprite_renderer;

    static void Initialize() { Instance()->Setup(); }
    void AddEntity(ClassId cid, int oid, std::unique_ptr<Entity> ent);
    void AddEntity(const PosInfo& posInfo);
    void RemoveEntity(ClassId cid, int oid);
    std::unique_ptr<Entity>& GetEntity(ClassId cid, int oid);

    void Update();

    void Draw();

    // Interface to camera
    glm::mat4 GetViewMatrix();
    glm::vec3 GetCameraPosition();
    glm::mat4 GetPerspectiveMatrix();

    std::unique_ptr<PointLight>& GetPointLight() { return m_pointLight; }
    Player* GetPlayer() { return player; }
    StaticObject* GetStaticObject(int i) { return m_staticObjects[i].get(); }
    DirectionalLight* GetDirectionalLight() { return m_dirLight.get(); }
    int NumStaticObjects() const { return m_staticObjects.size(); }
    bool IsRenderingDepth() const { return m_renderingDepthMap; }
    std::shared_ptr<Shader>& GetDepthShader() { return m_depthShader; }
    unsigned int ShadowMapIndex() const { return m_shadowMapIndex; }
    GLuint DepthMap() const { return m_depthMap; }
    const glm::mat4& LightSpaceMatrix() const
    {
        // lightSpaceMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f) *
        // GetViewMatrix();
        return m_lightSpaceMatrix;
    }

private:
    // helpers
    static glm::vec2 Get2D(glm::vec3 coords,
                           glm::mat4 view,
                           glm::mat4 projection /*perspective matrix */,
                           int width,
                           int height);
};
