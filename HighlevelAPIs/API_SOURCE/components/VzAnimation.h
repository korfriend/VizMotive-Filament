#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzAnimation : VzBaseComp
    {
        enum class LoopMode
        {
            ONCE,
            LOOP,
            PING_PONG
        };

        VzAnimation(const VID vid, const std::string& originFrom) :
            VzBaseComp(vid, originFrom, "VzAnimation") {}

        using AnimationVID = VID;

        //! Begins playing or restarts an animation
        void Play();

        //! Begins playing an animation from the start
        void PlayFromStart();

        //! Stops a running animation and resets time
        void Stop();

        //! Pauses a running animation
        void Pause();

        //! Reverses a running animation
        void Reverse();

        //! Reverses a running animation from the end
        void ReverseFromEnd();

        //! Returns whether or not the animation is playing
        bool IsPlaying();

        //! Returns whether or not the animation is reversing
        bool IsReversing();

        //! Sets the current time of the animation
        void SetTime(float time);

        //! Returns the current time of the animation
        float GetTime();

        //! Sets the loop mode of the animation
        void SetLoopMode(LoopMode mode);

        //! Returns the loop mode of the animation
        LoopMode GetLoopMode();

        //! Sets the play rate of the animation
        void SetPlayRate(float rate);

        //! Returns the play rate of the animation
        float GetPlayRate();

        //! Returns the duration of the animation
        float GetDuration();

        //! Sets the weight of the animation
        void SetWeight(float weight);

        //! Returns the weight of the animation
        float GetWeight();

        //! Cross fades from another animation
        void CrossFadeFrom(AnimationVID animation, float duration);

        //! Cross fades from another animation
        inline void CrossFadeFrom(VzAnimation* animation, float duration) { CrossFadeFrom(animation->GetVID(), duration); }

        //! Cross fades to another animation
        void CrossFadeTo(AnimationVID animation, float duration);

        inline void CrossFadeTo(VzAnimation* animation, float duration) { CrossFadeTo(animation->GetVID(), duration); }

        //! Fades in the animation
        void FadeIn(float duration);

        //! Fades out the animation
        void FadeOut(float duration);

        //! Stops fading
        void StopFading();
    };
}
