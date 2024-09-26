#include "VizCoreUtils.h"
#include "VzRenderPath.h"
#include "VzEngineApp.h"
#include "FIncludes.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

void vzm::helpers::ComputePosSS2WS(const uint32_t x, const uint32_t y, const float d, const VID camera, const VID renderer, float posWS[3])
{
    VzRenderPath* render_path = gEngineApp->GetRenderPath(renderer);
    if (render_path == nullptr) return;
    const filament::Viewport& vp = render_path->GetView()->getViewport();
    const Camera * cam = gEngine->getCameraComponent(utils::Entity::import(camera));
    if (cam == nullptr) return;
    auto inv_proj = mat4f(inverse(cam->getCullingProjectionMatrix()));
    auto inv_view = mat4f(cam->getModelMatrix());
    float x_ndc = 2.0f * (float) x / (float) vp.width - 1.0f;
    float y_ndc = 1.0f - 2.0f * (float) y / (float) vp.height;
    float4 p_ws_near_h = inv_view * inv_proj * float4(x_ndc, y_ndc, -1.0f, 1.0f);
    float4 p_ws_far_h = inv_view * inv_proj * float4(x_ndc, y_ndc, 1.0f, 1.0f);
    float3 p_ws_near = p_ws_near_h.xyz / p_ws_near_h.w;
    float3 p_ws_far = p_ws_far_h.xyz / p_ws_far_h.w;
    float3 p_ws = p_ws_near + (p_ws_far - p_ws_near) * clamp(d, 1e-10f, 1.0f);
    posWS[0] = p_ws.x;
    posWS[1] = p_ws.y;
    posWS[2] = p_ws.z;
}

void vzm::helpers::ComputePosSS2CS(const uint32_t x, const uint32_t y, const float d, const VID camera, const VID renderer, float posCS[3])
{
    VzRenderPath* render_path = gEngineApp->GetRenderPath(renderer);
    if (render_path == nullptr) return;
    const filament::Viewport& vp = render_path->GetView()->getViewport();
    const Camera * cam = gEngine->getCameraComponent(utils::Entity::import(camera));
    if (cam == nullptr) return;
    auto inv_proj = mat4f(inverse(cam->getCullingProjectionMatrix()));
    float x_ndc = 2.0f * (float) x / (float) vp.width - 1.0f;
    float y_ndc = 1.0f - 2.0f * (float) y / (float) vp.height;
    float4 p_cs_near_h = inv_proj * float4(x_ndc, y_ndc, -1.0f, 1.0f);
    float4 p_cs_far_h = inv_proj * float4(x_ndc, y_ndc, 1.0f, 1.0f);
    float3 p_cs_near = p_cs_near_h.xyz / p_cs_near_h.w;
    float3 p_cs_far = p_cs_far_h.xyz / p_cs_far_h.w;
    float3 p_cs = p_cs_near + (p_cs_far - p_cs_near) * clamp(d, 1e-10f, 1.0f);
    posCS[0] = p_cs.x;
    posCS[1] = p_cs.y;
    posCS[2] = p_cs.z;
}
