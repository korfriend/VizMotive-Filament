#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzAnimation : VzBaseComp
    {
        VzAnimation(const VID vid, const std::string& originFrom) :
            VzBaseComp(vid, originFrom, "VzAnimation") {}
    };
}
