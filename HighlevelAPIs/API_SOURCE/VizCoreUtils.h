#pragma once
#include "VizComponentAPIs.h"

namespace vzm::helpers
{
    void ComputePosSS2WS(const uint32_t x, const uint32_t y, const float d, const VID camera, const VID renderer, float posWS[3]);
    void ComputePosSS2CS(const uint32_t x, const uint32_t y, const float d, const VID camera, const VID renderer, float posCS[3]);
}
