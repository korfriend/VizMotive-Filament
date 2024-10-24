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
    };
}
