#pragma once

#include <string>
#include <glm/glm.hpp>

#include "AnimationMesh.h"
#include "AnimationPlayer.h"

namespace Animation
{
    class AnimatedModel
    {
        ///////////////////////////////////////////////////////////////////////
        // Member variables
        Mesh m_mesh;
        AnimationPlayer m_animPlayer;

        glm::mat4 m_toWorld;

        double lastTime = 0.0;        // TODO Remove

    public:
        ///////////////////////////////////////////////////////////////////////
        // Public methods
        AnimatedModel();
        AnimatedModel(std::string fbxFilename);

        std::string FBXLoadClean(std::string filename, bool animLoops = false, std::string animName = "");            // Loads both Mesh and Animation
        std::string AddAnimation(std::string animationFile, bool loops = false, std::string animName = "");   // Only loads Animation
        bool PlayAnimation(std::string name);
        bool SetAnimation(std::string name);

        bool ChangeMaterial(unsigned int index, Material& material);

        void Update(float deltaTime);
        void Draw(bool useShader = true) const;
        void InitBones0() { m_animPlayer.InitBones0(); }
        void Reset() { m_animPlayer.Reset(); }
        void ToggleAnimating() { m_animPlayer.ToggleAnimation(); }
        void RestartAnimation() { m_animPlayer.RestartAnimation(); }

        void RegisterListener(AnimationPlayer::Listener* listener) { m_animPlayer.RegisterListener(listener); }

        const Mesh& GetMesh() const { return m_mesh; }
        const AnimationPlayer& GetAnimPlayer() const { return m_animPlayer; }
    };
}
