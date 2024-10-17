#include "VzAnimator.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

#include "../backend/VzAnimatorImpl.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

namespace vzm
{
#define COMP_ANI(COMP, FAILRET)                       \
    VzAniRes* COMP = gEngineApp->GetAniRes(GetVID()); \
    if (COMP == nullptr)                              \
        return FAILRET;
#define COMP_ANI_IMPL(COMP, IMPL, FAILRET)    \
    COMP_ANI(COMP, FAILRET)                   \
    skm::AnimatorImpl* IMPL = COMP->animator; \
    if (IMPL == nullptr)                      \
        return FAILRET;

    VzAnimator::VzAnimator(const VID vid, const std::string& originFrom) :
        VzBaseComp(vid, originFrom, "VzAnimator")
    {
    }

    size_t VzAnimator::AddPlayScene(const VID vidScene) const
    {
        COMP_ANI(ani_res, 0)
        ani_res->associatedScenes.insert(vidScene);
        return ani_res->associatedScenes.size();
    }

    size_t VzAnimator::RemovePlayScene(const VID vidScene) const
    {
        COMP_ANI(ani_res, 0)
        ani_res->associatedScenes.erase(vidScene);
        return ani_res->associatedScenes.size();
    }

    bool VzAnimator::IsPlayScene(const VID vidScene) const
    {
        COMP_ANI(ani_res, false)
        return ani_res->associatedScenes.contains(vidScene);
    }

    size_t VzAnimator::GetAnimationCount() const
    {
        COMP_ANI_IMPL(ani_res, impl, 0)
        return impl->animations.size();
    }
    std::string VzAnimator::GetAnimationLabel(const int index) const
    {
        COMP_ANI_IMPL(ani_res, impl, "")
        if (index < 0 || index >= impl->animations.size())
            return "";
        return impl->animations[index].name;
    }
    std::vector<std::string> VzAnimator::GetAnimationLabels() const
    {
        COMP_ANI_IMPL(ani_res, impl, { { "" } })
        std::vector<std::string> labels;
        for (const auto& ani : impl->animations)
        {
            labels.push_back(ani.name);
        }
        return labels;
    }
    std::string VzAnimator::ActivateAnimation(const size_t index) const
    {
        COMP_ANI(ani_res, "")
        ani_res->activatedAnimations.insert(index);
        return GetAnimationLabel(static_cast<int>(index));
    }
    int VzAnimator::ActivateAnimationByLabel(const std::string& label) const
    {
        COMP_ANI_IMPL(ani_res, impl, -1)
        for (size_t i = 0; i < impl->animations.size(); ++i)
        {
            if (label == impl->animations[i].name)
            {
                ani_res->activatedAnimations.insert(i);
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    std::string VzAnimator::DeactivateAnimation(const size_t index) const
    {
        COMP_ANI(ani_res, "")
        ani_res->activatedAnimations.erase(index);
        return GetAnimationLabel(static_cast<int>(index));
    }
    int VzAnimator::DeactivateAnimationByLabel(const std::string& label) const
    {
        COMP_ANI_IMPL(ani_res, impl, -1)
        for (size_t i = 0; i < impl->animations.size(); ++i)
        {
            if (label == impl->animations[i].name)
            {
                ani_res->activatedAnimations.erase(i);
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    int VzAnimator::DeactivateAll() const
    {
        COMP_ANI_IMPL(ani_res, impl, -1)
        ani_res->activatedAnimations.clear();
        return static_cast<int>(impl->animations.size());
    }
    void VzAnimator::SetCrossFadeDuration(const int crossFadeAnimationIndex, const int crossFadePrevAnimationIndex, const double timeSec) const
    {
        COMP_ANI(ani_res, )
        ani_res->crossFadeAnimationIndex = crossFadeAnimationIndex;
        ani_res->crossFadePrevAnimationIndex = crossFadePrevAnimationIndex;
        ani_res->crossFadeDurationSec = timeSec;
    }
    float VzAnimator::GetAnimationPlayTime(const size_t index) const
    {
        COMP_ANI_IMPL(ani_res, impl, 0.f)
        if (index >= impl->animations.size())
            return 0.f;
        return impl->animations[index].duration;
    }
    float VzAnimator::GetAnimationPlayTimeByLabel(const std::string& label) const
    {
        COMP_ANI_IMPL(ani_res, impl, 0.f)
        for (const auto& ani : impl->animations)
        {
            if (label == ani.name)
            {
                return ani.duration;
            }
        }
        return 0.f;
    }

    void VzAnimator::MovePlayTime(const double elsapsedTimeSec) const
    {
        COMP_ANI(ani_res, )
        ani_res->elapsedTimeSec = elsapsedTimeSec;
    }

    double VzAnimator::GetPlayTime() const
    {
        COMP_ANI(ani_res, 0.)
        return ani_res->elapsedTimeSec;
    }

    void VzAnimator::SetPlayMode(const PlayMode playMode) const
    {
        COMP_ANI(ani_res, )
        ani_res->playMode = playMode;
        ani_res->resetAnimation = playMode == PlayMode::INIT_POSE;
    }

    VzAnimator::PlayMode VzAnimator::GetPlayMode() const
    {
        COMP_ANI(ani_res, PlayMode::INIT_POSE);
        return ani_res->playMode;
    }

    void VzAnimator::Reset() const
    {
        COMP_ANI(ani_res, )
        ani_res->resetAnimation = true;
    }

    void VzAnimator::ApplyAnimationTimeAt(const size_t index, const float elapsedTime) const
    {
        COMP_ANI_IMPL(ani_res, impl, )
        if (index >= impl->animations.size())
            return;

        using namespace skm;

        const Animation& anim = impl->animations[index];
        float time = fmod(elapsedTime, anim.duration);
        TransformManager& transformManager = *impl->transformManager;
        transformManager.openLocalTransformTransaction();
        for (const auto& channel : anim.channels)
        {
            const Sampler* sampler = channel.sourceData;
            if (sampler->times.size() < 2)
            {
                continue;
            }

            const TimeValues& times = sampler->times;

            // Find the first keyframe after the given time, or the keyframe that matches it exactly.
            TimeValues::const_iterator iter = times.lower_bound(time);

            // Compute the interpolant (between 0 and 1) and determine the keyframe pair.
            float t = 0.0f;
            size_t nextIndex;
            size_t prevIndex;
            if (iter == times.end())
            {
                nextIndex = times.size() - 1;
                prevIndex = nextIndex;
            }
            else if (iter == times.begin())
            {
                nextIndex = 0;
                prevIndex = 0;
            }
            else
            {
                TimeValues::const_iterator prev = iter;
                --prev;
                nextIndex = iter->second;
                prevIndex = prev->second;
                const float nextTime = iter->first;
                const float prevTime = prev->first;
                float deltaTime = nextTime - prevTime;
                assert(deltaTime >= 0);
                if (deltaTime > 0)
                {
                    t = (time - prevTime) / deltaTime;
                }
            }

            if (sampler->interpolation == Sampler::STEP)
            {
                t = 0.0f;
            }

            impl->applyAnimation(channel, t, prevIndex, nextIndex);
        }
        transformManager.commitLocalTransformTransaction();
    }

    void VzAnimator::UpdateBoneMatrices() const
    {
        COMP_ANI_IMPL(ani_res, impl, )
        impl->updateBoneMatrices(impl->skins);
    }

    void VzAnimator::UpdateAnimation() const
    {
        COMP_ANI_IMPL(ani_res, impl, )

        switch (ani_res->playMode)
        {
        case PlayMode::INIT_POSE:
            impl->resetBoneMatrices(impl->skins);
            ani_res->resetAnimation = true;
            return;
        case PlayMode::PAUSE:
            ani_res->timer = std::chrono::high_resolution_clock::now();
            return;
        case PlayMode::PLAY:
            break;
        }

        if (ani_res->resetAnimation)
        {
            ani_res->timer = std::chrono::high_resolution_clock::now();
            ani_res->prevElapsedTimeSec = ani_res->elapsedTimeSec;
            ani_res->elapsedTimeSec = 0;
            ani_res->resetAnimation = false;
            ani_res->crossFadeAnimationIndex = ani_res->crossFadePrevAnimationIndex = -1;
        }

        auto timestamp = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(timestamp - ani_res->timer);
        double delta_time = time_span.count(); // in sec.

        if (delta_time < ani_res->fixedUpdateTime)
        {
            //return; // WHY??? discontinuous?!
        }
        ani_res->timer = timestamp;
        ani_res->elapsedTimeSec += delta_time;

        const size_t animation_count = impl->animations.size();
        for (size_t i = 0; i < animation_count; ++i)
        {
            if (ani_res->activatedAnimations.contains(i))
            {
                ApplyAnimationTimeAt(i, static_cast<float>(ani_res->elapsedTimeSec));
            }
        }

        if (ani_res->elapsedTimeSec < ani_res->crossFadeDurationSec)
        {
            if (ani_res->crossFadeAnimationIndex >= 0 && ani_res->crossFadePrevAnimationIndex >= 0 && ani_res->crossFadeAnimationIndex != ani_res->crossFadePrevAnimationIndex)
            {
                const double previousSeconds = ani_res->prevElapsedTimeSec + delta_time;
                const auto lerpFactor = static_cast<float>(ani_res->elapsedTimeSec / ani_res->crossFadeDurationSec);
                ApplyAnimationTimeAt(ani_res->crossFadeAnimationIndex, static_cast<float>(ani_res->elapsedTimeSec));
                impl->stashCrossFade();
                ApplyAnimationTimeAt(ani_res->crossFadePrevAnimationIndex, static_cast<float>(previousSeconds));
                impl->applyCrossFade(lerpFactor);
            }
        }
        else
        {
            ani_res->crossFadeAnimationIndex = ani_res->crossFadePrevAnimationIndex = -1;
        }
        impl->updateBoneMatrices(impl->skins);
    }

}
