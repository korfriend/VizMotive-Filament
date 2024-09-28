#pragma once
#include "VizComponentAPIs.h"

namespace vzm::helpers
{
    void ComputePosSS2WS(const float x, const float y, const float d, const VID camera, const VID renderer, float posWS[3]);
    void ComputePosSS2CS(const float x, const float y, const float d, const VID camera, const VID renderer, float posCS[3]);
}
