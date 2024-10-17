#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzAnimator : VzBaseComp
    {
        enum class PlayMode
        {
            INIT_POSE,
            PLAY,
            PAUSE,
        };

        VzAnimator(const VID vid, const std::string& originFrom);

        size_t AddPlayScene(const VID vidScene) const;
        size_t RemovePlayScene(const VID vidScene) const;
        bool IsPlayScene(const VID vidScene) const;
        size_t GetAnimationCount() const;
        std::string GetAnimationLabel(const int index) const;
        std::vector<std::string> GetAnimationLabels() const;

        void ApplyAnimationTimeAt(const size_t index, const float elapsedTime) const;
        void UpdateBoneMatrices() const;

        std::string ActivateAnimation(const size_t index) const;
        int ActivateAnimationByLabel(const std::string& label) const;
        std::string DeactivateAnimation(const size_t index) const;
        int DeactivateAnimationByLabel(const std::string& label) const;
        int DeactivateAll() const;
        void SetCrossFadeDuration(const int crossFadeAnimationIndex, const int crossFadePrevAnimationIndex, const double timeSec = 1.) const;
        float GetAnimationPlayTime(const size_t index) const;
        float GetAnimationPlayTimeByLabel(const std::string& label) const;

        void MovePlayTime(const double elsapsedTimeSec) const;
        double GetPlayTime() const;
        void SetPlayMode(const PlayMode playMode) const;
        PlayMode GetPlayMode() const;
        void Reset() const;

        // note: this is called in the renderer (whose target is the associated scene) by default
        void UpdateAnimation() const;
    };
}
