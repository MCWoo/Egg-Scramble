#include "AnimatedModel.h"
#include "Basic/Utils.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> // Post processing flags

namespace Animation
{
    ///////////////////////////////////////////////////////////////////////
    AnimatedModel::AnimatedModel() : m_animPlayer(&m_mesh)
    {
        double lastTime = Utils::CurrentTime();
    }

    ///////////////////////////////////////////////////////////////////////
    AnimatedModel::AnimatedModel(std::string fbxFilename) : AnimatedModel()
    {
        FBXLoadClean(fbxFilename);
    }


    ///////////////////////////////////////////////////////////////////////
    // Load an FBX completely new. Loads mesh and animation
    std::string AnimatedModel::FBXLoadClean(std::string filename, bool animLoops, std::string animName)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!scene)   // If failed, return
        {
            return "";
        }
        
        // Should not fail
        m_mesh.InitFromScene(scene, filename);

        return m_animPlayer.AddAnimFromScene(scene, animLoops, animName);
    }

    ///////////////////////////////////////////////////////////////////////
    std::string AnimatedModel::AddAnimation(std::string animationFile, bool loops, std::string animName)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationFile.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!scene || !m_mesh.SameMesh(scene))   // If failed, return
        {
            return false;
        }
        
        return m_animPlayer.AddAnimFromScene(scene, loops, animName);
    }

    ///////////////////////////////////////////////////////////////////////
    bool AnimatedModel::PlayAnimation(std::string name)
    {
        return m_animPlayer.PlayAnimation(name);
    }

    ///////////////////////////////////////////////////////////////////////
    bool AnimatedModel::SetAnimation(std::string name)
    {
        return m_animPlayer.SetAnimation(name);
    }

    ///////////////////////////////////////////////////////////////////////
    bool AnimatedModel::ChangeMaterial(unsigned int index, Material& material)
    {
        if (index >= m_mesh.m_materials.size())
        {
            return false;
        }

        m_mesh.m_materials[index] = material;
        return true;
    }

    ///////////////////////////////////////////////////////////////////////
    void AnimatedModel::Update(float deltaTime)
    {
        //double nextTime = Utils::CurrentTime();
        //float delta = (float)(nextTime - lastTime);
        //lastTime = nextTime;
        m_animPlayer.Update(deltaTime);
    }

    ///////////////////////////////////////////////////////////////////////
    void AnimatedModel::Draw(bool useShader) const
    {
        m_mesh.Draw(useShader);
    }
}