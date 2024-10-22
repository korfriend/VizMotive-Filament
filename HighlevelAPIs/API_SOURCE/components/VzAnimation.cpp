#include "VzAnimation.h"
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
    void VzAnimation::Play()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = false;
    }
    void VzAnimation::PlayFromStart()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = false;
        ani_res->currentTime = 0.0f;
    }
    void VzAnimation::Stop()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = false;
        ani_res->isReversing = false;
        ani_res->currentTime = 0.0f;
    }
    void VzAnimation::Pause()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = false;
    }
    void VzAnimation::Reverse()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = true;
    }
    void VzAnimation::ReverseFromEnd()
    {
        COMP_ANI(ani_res, );
        ani_res->isPlaying = true;
        ani_res->isReversing = true;
        ani_res->currentTime = ani_res->animation->duration;
    }
    bool VzAnimation::IsPlaying()
    {
        COMP_ANI(ani_res, false);
        return ani_res->isPlaying;
    }
    bool VzAnimation::IsReversing()
    {
        COMP_ANI(ani_res, false);
        return ani_res->isPlaying && ani_res->isReversing;
    }
    void VzAnimation::SetCurrentTime(float time)
    {
        COMP_ANI(ani_res, );
        ani_res->currentTime = time;
    }
    float VzAnimation::GetCurrentTime()
    {
        COMP_ANI(ani_res, 0.0f);
        return ani_res->currentTime;
    }
    void VzAnimation::SetLoopMode(LoopMode mode)
    {
        COMP_ANI(ani_res, );
        ani_res->loopMode = mode;
    }
    VzAnimation::LoopMode VzAnimation::GetLoopMode()
    {
        COMP_ANI(ani_res, LoopMode::LOOP);
        return ani_res->loopMode;
    }
    void VzAnimation::SetPlayRate(float rate)
    {
        COMP_ANI(ani_res, );
        ani_res->playRate = rate;
    }
    float VzAnimation::GetPlayRate()
    {
        COMP_ANI(ani_res, 1.0f);
        return ani_res->playRate;
    }
    float VzAnimation::GetDuration()
    {
        COMP_ANI(ani_res, 0.0f);
        return ani_res->animation->duration;
    }
}
