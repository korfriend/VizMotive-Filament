#include "VzAnimation.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

#include "../backend/VzAnimator.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

namespace vzm
{
#define COMP_ANIMATION(COMP, FAILRET)                 \
    VzAniRes* COMP = gEngineApp->GetAniRes(GetVID()); \
    if (COMP == nullptr)                              \
        return FAILRET;
    void VzAnimation::Play()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = false;
    }
    void VzAnimation::PlayFromStart()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = false;
        ani_res->currentTime = 0.0f;
    }
    void VzAnimation::Stop()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = false;
        ani_res->isReversing = false;
        ani_res->currentTime = 0.0f;
    }
    void VzAnimation::Pause()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = false;
    }
    void VzAnimation::Reverse()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = true;
    }
    void VzAnimation::ReverseFromEnd()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = true;
        ani_res->currentTime = ani_res->animation->duration;
    }
    bool VzAnimation::IsPlaying()
    {
        COMP_ANIMATION(ani_res, false);
        return ani_res->isPlaying;
    }
    bool VzAnimation::IsReversing()
    {
        COMP_ANIMATION(ani_res, false);
        return ani_res->isPlaying && ani_res->isReversing;
    }
    void VzAnimation::SetTime(float time)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->currentTime = time;
    }
    float VzAnimation::GetTime()
    {
        COMP_ANIMATION(ani_res, 0.0f);
        return ani_res->currentTime;
    }
    void VzAnimation::SetLoopMode(LoopMode mode)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->loopMode = mode;
    }
    VzAnimation::LoopMode VzAnimation::GetLoopMode()
    {
        COMP_ANIMATION(ani_res, LoopMode::LOOP);
        return ani_res->loopMode;
    }
    void VzAnimation::SetPlayRate(float rate)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->playRate = rate;
    }
    float VzAnimation::GetPlayRate()
    {
        COMP_ANIMATION(ani_res, 1.0f);
        return ani_res->playRate;
    }
    float VzAnimation::GetDuration()
    {
        COMP_ANIMATION(ani_res, 0.0f);
        return ani_res->animation->duration;
    }
    void VzAnimation::SetWeight(float weight)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->weight = weight;
    }
    float VzAnimation::GetWeight()
    {
        COMP_ANIMATION(ani_res, 1.0f);
        return ani_res->weight;
    }
    void VzAnimation::CrossFadeFrom(AnimationVID animation, float duration)
    {
        FadeIn(duration);
        gEngineApp->GetVzComponent<VzAnimation>(animation)->FadeOut(duration);
    }
    void VzAnimation::CrossFadeTo(AnimationVID animation, float duration)
    {
        FadeOut(duration);
        gEngineApp->GetVzComponent<VzAnimation>(animation)->FadeIn(duration);
    }
    void VzAnimation::FadeIn(float duration)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->weight = 0.0f;
        ani_res->fadeDuration = duration;
        ani_res->isFadeOut = false;
    }
    void VzAnimation::FadeOut(float duration)
    {
        COMP_ANIMATION(ani_res, );
        ani_res->weight = 1.0f;
        ani_res->fadeDuration = duration;
        ani_res->isFadeOut = true;
    }
    void VzAnimation::StopFading()
    {
        COMP_ANIMATION(ani_res, );
        ani_res->fadeDuration = 0.0f;
    }
}
