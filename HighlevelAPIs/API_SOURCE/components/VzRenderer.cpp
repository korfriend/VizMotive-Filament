#include "VzRenderer.h"
#include "../VzRenderPath.h"
#include "../VzEngineApp.h"
#include "VzAsset.h"
#include "../FIncludes.h"
#include "../VizCoreUtils.h"

#include "../../filament/src/PostProcessManager.h"

#include "../backend/VzAnimator.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

namespace vzm
{
    void VzRenderer::SetCanvas(const uint32_t w, const uint32_t h, const float dpi, void* window)
    {
        COMP_RENDERPATH(render_path, );

        std::vector<RendererVID> render_path_vids;
        gEngineApp->GetRenderPathVids(render_path_vids);
        if (window)
        {
            for (size_t i = 0, n = render_path_vids.size(); i < n; ++i)
            {
                VID vid_i = render_path_vids[i];
                VzRenderPath* render_path_i = gEngineApp->GetRenderPath(vid_i);
                if (render_path_i != render_path)
                {
                    void* window_i = nullptr;
                    uint32_t w_i, h_i;
                    float dpi_i;
                    render_path_i->GetCanvas(&w_i, &h_i, &dpi_i, &window_i);
                    if (window_i == window)
                    {
                        std::string name_i = gEngineApp->GetVzComponent<VzRenderer>(vid_i)->GetName();
                        render_path_i->SetCanvas(w_i, h_i, dpi_i, nullptr);
                        backlog::post("another renderer (" + name_i + ") has the same window handle, so force to set nullptr!", backlog::LogLevel::Warning);
                    }
                }
            }
        }

        render_path->SetCanvas(w, h, dpi, window);
        UpdateTimeStamp();
    }
    void VzRenderer::GetCanvas(uint32_t* w, uint32_t* h, float* dpi, void** window)
    {
        COMP_RENDERPATH(render_path, );
        render_path->GetCanvas(w, h, dpi, window);
    }

    void VzRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h)
    {
        COMP_RENDERPATH(render_path, );
        uint32_t canvas_h;
        render_path->GetCanvas(nullptr, &canvas_h, nullptr, nullptr);
        render_path->SetViewport(x, canvas_h - y - h, w, h);
    }

    void VzRenderer::GetViewport(uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h)
    {
        COMP_RENDERPATH(render_path, );
        const filament::Viewport& vp = render_path->GetView()->getViewport();
        if (x)
            *x = vp.left;
        if (y)
            *y = vp.bottom;
        if (w)
            *w = vp.width;
        if (h)
            *h = vp.height;
    }

    void VzRenderer::SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits)
    {
        COMP_RENDERPATH(render_path, );
        View* view = render_path->GetView();
        view->setVisibleLayers(layerBits, maskBits);
        UpdateTimeStamp();
    }

    void VzRenderer::Pick(const uint32_t x, const uint32_t y, PickCallback callback)
    {
        COMP_RENDERPATH(render_path, );
        View* view = render_path->GetView();
        uint32_t canvas_h;
        render_path->GetCanvas(nullptr, &canvas_h, nullptr, nullptr);
        const filament::Viewport& vp = view->getViewport();
        uint32_t x_ = x - vp.left;
        uint32_t y_ = (canvas_h - y) - vp.bottom;
        if (x_ >= vp.width || y_ >= vp.height)
            return;
        view->pick(x_, y_, [callback](View::PickingQueryResult const& result) {
            callback(result.renderable.getId());
        });
    }

    size_t VzRenderer::IntersectActors(const uint32_t x, const uint32_t y, const VID vidCam, const std::vector<VID>& vidActors, std::vector<HitResult>& results, const bool recursive)
    {
        COMP_RENDERPATH(render_path, 0);
        results.clear();
        const Camera* camera = gEngine->getCameraComponent(utils::Entity::import(vidCam));
        if (camera == nullptr)
            return 0;
        uint32_t canvas_h;
        render_path->GetCanvas(nullptr, &canvas_h, nullptr, nullptr);
        const filament::Viewport& vp = render_path->GetView()->getViewport();
        uint32_t x_ = x - vp.left;
        uint32_t y_ = vp.height - ((canvas_h - y) - vp.bottom);
        if (x_ >= vp.width || y_ >= vp.height)
            return 0;
        float3 p_ws;
        helpers::ComputePosSS2WS(x_, y_, 0.0f, vidCam, GetVID(), __FP p_ws);
        float3 rayOrigin = camera->getPosition();
        float3 rayDirection = normalize(p_ws - rayOrigin);
        std::function<void(VID)> intersect = [&](const VID vidActor) {
            VzSceneComp* actor = gEngineApp->GetVzComponent<VzSceneComp>(vidActor);
            if (actor == nullptr)
            {
                backlog::post("actor is nullptr", backlog::LogLevel::Error);
                return;
            }
            bool result = false;
            switch (actor->GetSceneCompType())
            {
            case SCENE_COMPONENT_TYPE::SPRITE_ACTOR:
                result = ((VzSpriteActor*)actor)->Raycast(__FP rayOrigin, __FP rayDirection, results);
                break;
            case SCENE_COMPONENT_TYPE::TEXT_SPRITE_ACTOR:
                result = ((VzTextSpriteActor*)actor)->Raycast(__FP rayOrigin, __FP rayDirection, results);
                break;
            default:
                backlog::post("scene component type is not supported : " + std::to_string((int)actor->GetSceneCompType()), backlog::LogLevel::Error);
                break;
            }
            if (result && recursive)
            {
                std::vector<VID> vidChildren = actor->GetChildren();
                for (auto vidChild : vidChildren)
                {
                    intersect(vidChild);
                }
            }
        };
        for (auto vidActor : vidActors)
        {
            intersect(vidActor);
        }
        std::sort(results.begin(), results.end(), [](const HitResult& lhs, const HitResult& rhs) {
            return lhs.distance < rhs.distance;
        });
        UpdateTimeStamp();
        return results.size();
    }

#pragma region View
    void VzRenderer::SetPostProcessingEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.postProcessingEnabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::POST_PROCESSING_ENABLED;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsPostProcessingEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.postProcessingEnabled;
    }
    void VzRenderer::SetDitheringEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dithering = enabled ? Dithering::TEMPORAL : Dithering::NONE;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DITHERING;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDitheringEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.dithering == Dithering::TEMPORAL;
    }
    void VzRenderer::SetBloomEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsBloomEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.bloom.enabled;
    }
    void VzRenderer::SetTaaEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.enabled;
    }
    void VzRenderer::SetFxaaEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.antiAliasing = enabled ? AntiAliasing::FXAA : AntiAliasing::NONE;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::ANTI_ALIASING;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsFxaaEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.antiAliasing == AntiAliasing::FXAA;
    }
    void VzRenderer::SetMsaaEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.msaa.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::MSAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsMsaaEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.msaa.enabled;
    }
    void VzRenderer::SetMsaaSampleCount(int samples)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.msaa.sampleCount = samples;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::MSAA;
        UpdateTimeStamp();
    }
    int VzRenderer::GetMsaaSampleCount()
    {
        COMP_RENDERPATH(render_path, 4);
        return render_path->viewSettings.msaa.sampleCount;
    }
    void VzRenderer::SetMsaaCustomResolve(bool customResolve)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.msaa.customResolve = customResolve;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::MSAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsMsaaCustomResolve()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.msaa.customResolve;
    }
    void VzRenderer::SetSsaoEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsSsaoEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.ssao.enabled;
    }
    void VzRenderer::SetScreenSpaceReflectionsEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.screenSpaceReflections.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SCREEN_SPACE_REFLECTIONS;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsScreenSpaceReflectionsEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.screenSpaceReflections.enabled;
    }
    void VzRenderer::SetGuardBandEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.guardBand.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::GUARD_BAND;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsGuardBandEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.guardBand.enabled;
    }
#pragma endregion

#pragma region Bloom Options
    void VzRenderer::SetBloomStrength(float strength)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.strength = strength;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    float VzRenderer::GetBloomStrength()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.bloom.strength;
    }
    void VzRenderer::SetBloomThreshold(bool threshold)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.threshold = threshold;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsBloomThreshold()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.bloom.threshold;
    }
    void VzRenderer::SetBloomLevels(int levels)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.levels = levels;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    int VzRenderer::GetBloomLevels()
    {
        COMP_RENDERPATH(render_path, (int)QualityLevel::LOW);
        return render_path->viewSettings.bloom.levels;
    }
    void VzRenderer::SetBloomQuality(int quality)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.quality = (QualityLevel)std::clamp(quality, 0, 3);
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    int VzRenderer::GetBloomQuality()
    {
        COMP_RENDERPATH(render_path, 0);
        return (int)render_path->viewSettings.bloom.quality;
    }
    void VzRenderer::SetBloomLensFlare(bool lensFlare)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.bloom.lensFlare = lensFlare;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::BLOOM;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsBloomLensFlare()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.bloom.lensFlare;
    }
#pragma endregion

#pragma region TAA Options
    void VzRenderer::SetTaaUpscaling(bool upscaling)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.upscaling = upscaling;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaUpscaling()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.upscaling;
    }
    void VzRenderer::SetTaaHistoryReprojection(bool historyReprojection)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.historyReprojection = historyReprojection;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaHistoryReprojection()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.historyReprojection;
    }
    void VzRenderer::SetTaaFeedback(float feedback)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.feedback = feedback;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    float VzRenderer::GetTaaFeedback()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.taa.feedback;
    }
    void VzRenderer::SetTaaFilterHistory(bool filterHistory)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.filterHistory = filterHistory;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaFilterHistory()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.filterHistory;
    }
    void VzRenderer::SetTaaFilterInput(bool filterInput)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.filterInput = filterInput;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaFilterInput()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.filterInput;
    }
    void VzRenderer::SetTaaFilterWidth(float filterWidth)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.filterWidth = filterWidth;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    float VzRenderer::GetTaaFilterWidth()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.taa.filterWidth;
    }
    void VzRenderer::SetTaaLodBias(float lodBias)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.lodBias = lodBias;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    float VzRenderer::GetTaaLodBias()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.taa.lodBias;
    }
    void VzRenderer::SetTaaUseYCoCg(bool useYCoCg)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.useYCoCg = useYCoCg;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaUseYCoCg()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.useYCoCg;
    }
    void VzRenderer::SetTaaPreventFlickering(bool preventFlickering)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.preventFlickering = preventFlickering;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsTaaPreventFlickering()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.taa.preventFlickering;
    }
    void VzRenderer::SetTaaJitterPattern(JitterPattern pattern)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.jitterPattern = (TemporalAntiAliasingOptions::JitterPattern)pattern;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    VzRenderer::JitterPattern VzRenderer::GetTaaJitterPattern()
    {
        COMP_RENDERPATH(render_path, JitterPattern::HALTON_23_X16);
        return (JitterPattern)render_path->viewSettings.taa.jitterPattern;
    }
    void VzRenderer::SetTaaBoxClipping(BoxClipping boxClipping)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.boxClipping = (TemporalAntiAliasingOptions::BoxClipping)boxClipping;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    VzRenderer::BoxClipping VzRenderer::GetTaaBoxClipping()
    {
        COMP_RENDERPATH(render_path, BoxClipping::ACCURATE);
        return (BoxClipping)render_path->viewSettings.taa.boxClipping;
    }
    void VzRenderer::SetTaaBoxType(BoxType boxType)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.boxType = (TemporalAntiAliasingOptions::BoxType)boxType;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    VzRenderer::BoxType VzRenderer::GetTaaBoxType()
    {
        COMP_RENDERPATH(render_path, BoxType::AABB);
        return (BoxType)render_path->viewSettings.taa.boxType;
    }
    void VzRenderer::SetTaaVarianceGamma(float varianceGamma)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.varianceGamma = varianceGamma;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    float VzRenderer::GetTaaVarianceGamma()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.taa.varianceGamma;
    }
    void VzRenderer::SetTaaSharpness(float sharpness)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.taa.sharpness = sharpness;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::TAA;
        UpdateTimeStamp();
    }
    float VzRenderer::GetTaaSharpness()
    {
        COMP_RENDERPATH(render_path, 0);
        return render_path->viewSettings.taa.sharpness;
    }
#pragma endregion

#pragma region SSAO Options
    void VzRenderer::SetSsaoQuality(int quality)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.quality = (QualityLevel)std::clamp(quality, 0, 3);
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    int VzRenderer::GetSsaoQuality()
    {
        COMP_RENDERPATH(render_path, (int)QualityLevel::LOW);
        return (int)render_path->viewSettings.ssao.quality;
    }
    void VzRenderer::SetSsaoLowPassFilter(int lowPassFilter)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.lowPassFilter = (QualityLevel)std::clamp(lowPassFilter, 0, 2);
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    int VzRenderer::GetSsaoLowPassFilter()
    {
        COMP_RENDERPATH(render_path, (int)QualityLevel::MEDIUM);
        return (int)render_path->viewSettings.ssao.lowPassFilter;
    }
    void VzRenderer::SetSsaoBentNormals(bool bentNormals)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.bentNormals = bentNormals;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsSsaoBentNormals()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.ssao.bentNormals;
    }
    void VzRenderer::SetSsaoUpsampling(bool upsampling)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.upsampling = upsampling ? QualityLevel::HIGH : QualityLevel::LOW;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsSsaoUpsampling()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.ssao.upsampling != QualityLevel::LOW;
    }
    void VzRenderer::SetSsaoMinHorizonAngleRad(float minHorizonAngleRad)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.minHorizonAngleRad = minHorizonAngleRad;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoMinHorizonAngleRad()
    {
        COMP_RENDERPATH(render_path, 0.0f);
        return render_path->viewSettings.ssao.minHorizonAngleRad;
    }
    void VzRenderer::SetSsaoBilateralThreshold(float bilateralThreshold)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.bilateralThreshold = bilateralThreshold;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoBilateralThreshold()
    {
        COMP_RENDERPATH(render_path, 0.05f);
        return render_path->viewSettings.ssao.bilateralThreshold;
    }
    void VzRenderer::SetSsaoHalfResolution(bool halfResolution)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.resolution = halfResolution ? 0.5f : 1.0f;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsSsaoHalfResolution()
    {
        COMP_RENDERPATH(render_path, true);
        return render_path->viewSettings.ssao.resolution != 1.0f;
    }

#pragma region Dominant Light Shadows (experimental)
    void VzRenderer::SetSsaoSsctEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsSsaoSsctEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.ssao.ssct.enabled;
    }
    void VzRenderer::SetSsaoSsctLightConeRad(float lightConeRad)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.lightConeRad = lightConeRad;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctLightConeRad()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.ssao.ssct.lightConeRad;
    }
    void VzRenderer::SetSsaoSsctShadowDistance(float shadowDistance)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.shadowDistance = shadowDistance;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctShadowDistance()
    {
        COMP_RENDERPATH(render_path, 0.3f);
        return render_path->viewSettings.ssao.ssct.shadowDistance;
    }
    void VzRenderer::SetSsaoSsctContactDistanceMax(float contactDistanceMax)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.contactDistanceMax = contactDistanceMax;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctContactDistanceMax()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.ssao.ssct.contactDistanceMax;
    }
    void VzRenderer::SetSsaoSsctIntensity(float intensity)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.intensity = intensity;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctIntensity()
    {
        COMP_RENDERPATH(render_path, 0.8f);
        return render_path->viewSettings.ssao.ssct.intensity;
    }
    void VzRenderer::SetSsaoSsctDepthBias(float depthBias)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.depthBias = depthBias;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctDepthBias()
    {
        COMP_RENDERPATH(render_path, 0.01f);
        return render_path->viewSettings.ssao.ssct.depthBias;
    }
    void VzRenderer::SetSsaoSsctDepthSlopeBias(float depthSlopeBias)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.depthSlopeBias = depthSlopeBias;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSsaoSsctDepthSlopeBias()
    {
        COMP_RENDERPATH(render_path, 0.01f);
        return render_path->viewSettings.ssao.ssct.depthSlopeBias;
    }
    void VzRenderer::SetSsaoSsctSampleCount(int sampleCount)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.sampleCount = (uint8_t)sampleCount;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    int VzRenderer::GetSsaoSsctSampleCount()
    {
        COMP_RENDERPATH(render_path, 4);
        return render_path->viewSettings.ssao.ssct.sampleCount;
    }
    void VzRenderer::SetSsaoSsctLightDirection(const float lightDirection[3])
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.ssao.ssct.lightDirection.x = lightDirection[0];
        render_path->viewSettings.ssao.ssct.lightDirection.y = lightDirection[1];
        render_path->viewSettings.ssao.ssct.lightDirection.z = lightDirection[2];
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SSAO;
        UpdateTimeStamp();
    }
    void VzRenderer::GetSsaoSsctLightDirection(float lightDirection[3])
    {
        COMP_RENDERPATH(render_path, );
        lightDirection[0] = render_path->viewSettings.ssao.ssct.lightDirection.x;
        lightDirection[1] = render_path->viewSettings.ssao.ssct.lightDirection.y;
        lightDirection[2] = render_path->viewSettings.ssao.ssct.lightDirection.z;
    }
#pragma endregion

#pragma endregion

#pragma region Screen-space reflections Options
    void VzRenderer::SetScreenSpaceReflectionsThickness(float thickness)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.screenSpaceReflections.thickness = thickness;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SCREEN_SPACE_REFLECTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetScreenSpaceReflectionsThickness()
    {
        COMP_RENDERPATH(render_path, 0.1f);
        return render_path->viewSettings.screenSpaceReflections.thickness;
    }
    void VzRenderer::SetScreenSpaceReflectionsBias(float bias)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.screenSpaceReflections.bias = bias;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SCREEN_SPACE_REFLECTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetScreenSpaceReflectionsBias()
    {
        COMP_RENDERPATH(render_path, 0.01f);
        return render_path->viewSettings.screenSpaceReflections.bias;
    }
    void VzRenderer::SetScreenSpaceReflectionsMaxDistance(float maxDistance)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.screenSpaceReflections.maxDistance = maxDistance;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SCREEN_SPACE_REFLECTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetScreenSpaceReflectionsMaxDistance()
    {
        COMP_RENDERPATH(render_path, 3.0f);
        return render_path->viewSettings.screenSpaceReflections.maxDistance;
    }
    void VzRenderer::SetScreenSpaceReflectionsStride(float stride)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.screenSpaceReflections.stride = stride;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SCREEN_SPACE_REFLECTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetScreenSpaceReflectionsStride()
    {
        COMP_RENDERPATH(render_path, 2.0f);
        return render_path->viewSettings.screenSpaceReflections.stride;
    }
#pragma endregion

#pragma region Dynamic Resolution
    void VzRenderer::SetDynamicResoultionEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDynamicResoultionEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.dsr.enabled;
    }
    void VzRenderer::SetDynamicResoultionHomogeneousScaling(bool homogeneousScaling)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.homogeneousScaling = homogeneousScaling;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDynamicResoultionHomogeneousScaling()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.dsr.homogeneousScaling;
    }
    void VzRenderer::SetDynamicResoultionMinScale(float minScale)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.minScale = minScale;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    float VzRenderer::GetDynamicResoultionMinScale()
    {
        COMP_RENDERPATH(render_path, 0.5f);
        return render_path->viewSettings.dsr.minScale.x;
    }
    void VzRenderer::SetDynamicResoultionMaxScale(float maxScale)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.maxScale = maxScale;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    float VzRenderer::GetDynamicResoultionMaxScale()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.dsr.maxScale.x;
    }
    void VzRenderer::SetDynamicResoultionQuality(int quality)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.quality = (QualityLevel)std::clamp(quality, 0, 3);
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    int VzRenderer::GetDynamicResoultionQuality()
    {
        COMP_RENDERPATH(render_path, 0);
        return (int)render_path->viewSettings.dsr.quality;
    }
    void VzRenderer::SetDynamicResoultionSharpness(float sharpness)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dsr.sharpness = sharpness;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DSR;
        UpdateTimeStamp();
    }
    float VzRenderer::GetDynamicResoultionSharpness()
    {
        COMP_RENDERPATH(render_path, 0.9f);
        return render_path->viewSettings.dsr.sharpness;
    }
#pragma endregion

#pragma region Shadows
    void VzRenderer::SetShadowType(ShadowType shadowType)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.shadowType = (filament::ShadowType)shadowType;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VSM_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    VzRenderer::ShadowType VzRenderer::GetShadowType()
    {
        COMP_RENDERPATH(render_path, ShadowType::PCF);
        return (ShadowType)render_path->viewSettings.shadowType;
    }
    void VzRenderer::SetVsmHighPrecision(bool highPrecision)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vsmShadowOptions.highPrecision = highPrecision;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VSM_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsVsmHighPrecision()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.vsmShadowOptions.highPrecision;
    }
    void VzRenderer::SetVsmMsaaSamples(int msaaSamples)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vsmShadowOptions.msaaSamples = msaaSamples;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VSM_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    int VzRenderer::GetVsmMsaaSamples()
    {
        COMP_RENDERPATH(render_path, 1);
        return render_path->viewSettings.vsmShadowOptions.msaaSamples;
    }
    void VzRenderer::SetVsmAnisotropy(int anisotropy)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vsmShadowOptions.anisotropy = anisotropy;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VSM_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    int VzRenderer::GetVsmAnisotropy()
    {
        COMP_RENDERPATH(render_path, 1);
        return render_path->viewSettings.vsmShadowOptions.anisotropy;
    }
    void VzRenderer::SetVsmMipmapping(bool mipmapping)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vsmShadowOptions.mipmapping = mipmapping;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VSM_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsVsmMipmapping()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.vsmShadowOptions.mipmapping;
    }
    void VzRenderer::SetSoftShadowPenumbraScale(float penumbraScale)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.softShadowOptions.penumbraScale = penumbraScale;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SOFT_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSoftShadowPenumbraScale()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.softShadowOptions.penumbraScale;
    }
    void VzRenderer::SetSoftShadowPenumbraRatioScale(float penumbraRatioScale)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.softShadowOptions.penumbraRatioScale = penumbraRatioScale;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::SOFT_SHADOW_OPTIONS;
        UpdateTimeStamp();
    }
    float VzRenderer::GetSoftShadowPenumbraRatioScale()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.softShadowOptions.penumbraRatioScale;
    }
#pragma endregion

#pragma region Fog
    void VzRenderer::SetFogEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsFogEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.fog.enabled;
    }
    void VzRenderer::SetFogDistance(float distance)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.distance = distance;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogDistance()
    {
        COMP_RENDERPATH(render_path, 0.0f);
        return render_path->viewSettings.fog.distance;
    }
    void VzRenderer::SetFogDensity(float density)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.density = density;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogDensity()
    {
        COMP_RENDERPATH(render_path, 0.1f);
        return render_path->viewSettings.fog.density;
    }
    void VzRenderer::SetFogHeight(float height)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.height = height;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogHeight()
    {
        COMP_RENDERPATH(render_path, 0.0f);
        return render_path->viewSettings.fog.height;
    }
    void VzRenderer::SetFogHeightFalloff(float heightFalloff)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.heightFalloff = heightFalloff;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogHeightFalloff()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.fog.heightFalloff;
    }
    void VzRenderer::SetFogInScatteringStart(float inScatteringStart)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.inScatteringStart = inScatteringStart;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogInScatteringStart()
    {
        COMP_RENDERPATH(render_path, 0.0f);
        return render_path->viewSettings.fog.inScatteringStart;
    }
    void VzRenderer::SetFogInScatteringSize(float inScatteringSize)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.inScatteringSize = inScatteringSize;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    float VzRenderer::GetFogInScatteringSize()
    {
        COMP_RENDERPATH(render_path, -1.0f);
        return render_path->viewSettings.fog.inScatteringSize;
    }
    void VzRenderer::SetFogExcludeSkybox(bool excludeSkybox)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.cutOffDistance =
            excludeSkybox ? 1e6f : std::numeric_limits<float>::infinity();
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsFogExcludeSkybox()
    {
        COMP_RENDERPATH(render_path, false);
        return !std::isinf(render_path->viewSettings.fog.cutOffDistance);
    }
    void VzRenderer::SetFogColorSource(FogColorSource fogColorSource)
    {
        COMP_RENDERPATH(render_path, );
        switch (fogColorSource)
        {
        case FogColorSource::CONSTANT:
            render_path->viewSettings.fog.skyColor = nullptr;
            render_path->viewSettings.fog.fogColorFromIbl = false;
            break;
        case FogColorSource::IBL:
            render_path->viewSettings.fog.skyColor = nullptr;
            render_path->viewSettings.fog.fogColorFromIbl = true;
            break;
        case FogColorSource::SKYBOX:
            render_path->viewSettings.fog.skyColor = render_path->viewSettings.fogSettings.fogColorTexture;
            render_path->viewSettings.fog.fogColorFromIbl = false;
            break;
        }
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    VzRenderer::FogColorSource VzRenderer::GetFogColorSource()
    {
        COMP_RENDERPATH(render_path, FogColorSource::CONSTANT);
        FogColorSource fogColorSource = FogColorSource::CONSTANT;
        if (render_path->viewSettings.fog.skyColor)
        {
            fogColorSource = FogColorSource::SKYBOX;
        }
        else if (render_path->viewSettings.fog.fogColorFromIbl)
        {
            fogColorSource = FogColorSource::IBL;
        }
        return fogColorSource;
    }
    void VzRenderer::SetFogColor(const float color[3])
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.fog.color.r = color[0];
        render_path->viewSettings.fog.color.g = color[1];
        render_path->viewSettings.fog.color.b = color[2];
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::FOG;
        UpdateTimeStamp();
    }
    void VzRenderer::GetFogColor(float color[3])
    {
        COMP_RENDERPATH(render_path, );
        color[0] = render_path->viewSettings.fog.color.r;
        color[1] = render_path->viewSettings.fog.color.g;
        color[2] = render_path->viewSettings.fog.color.b;
    }
#pragma endregion

#pragma region DoF
    void VzRenderer::SetDofEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDofEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.dof.enabled;
    }
    void VzRenderer::SetDofCocScale(float cocScale)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.cocScale = cocScale;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    float VzRenderer::GetDofCocScale()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.dof.cocScale;
    }
    void VzRenderer::SetDofCocAspectRatio(float cocAspectRatio)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.cocAspectRatio = cocAspectRatio;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    float VzRenderer::GetDofCocAspectRatio()
    {
        COMP_RENDERPATH(render_path, 1.0f);
        return render_path->viewSettings.dof.cocAspectRatio;
    }
    void VzRenderer::SetDofRingCount(int dofRingCount)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.backgroundRingCount = (uint8_t)dofRingCount;
        render_path->viewSettings.dof.foregroundRingCount = (uint8_t)dofRingCount;
        render_path->viewSettings.dof.fastGatherRingCount = (uint8_t)dofRingCount;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    int VzRenderer::GetDofRingCount()
    {
        COMP_RENDERPATH(render_path, 5);
        return render_path->viewSettings.dof.fastGatherRingCount;
    }
    void VzRenderer::SetDofMaxCoc(int maxCoc)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.maxForegroundCOC = (uint16_t)maxCoc;
        render_path->viewSettings.dof.maxBackgroundCOC = (uint16_t)maxCoc;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    int VzRenderer::GetDofMaxCoc()
    {
        COMP_RENDERPATH(render_path, 32);
        return render_path->viewSettings.dof.maxForegroundCOC;
    }
    void VzRenderer::SetDofNativeResolution(bool nativeResolution)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.nativeResolution = nativeResolution;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDofNativeResolution()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.dof.nativeResolution;
    }
    void VzRenderer::SetDofMedian(bool dofMedian)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.dof.filter = dofMedian ? DepthOfFieldOptions::Filter::MEDIAN : DepthOfFieldOptions::Filter::NONE;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::DOF;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsDofMedian()
    {
        COMP_RENDERPATH(render_path, true);
        return render_path->viewSettings.dof.filter == DepthOfFieldOptions::Filter::MEDIAN;
    }
#pragma endregion

#pragma region Vignette
    void VzRenderer::SetVignetteEnabled(bool enabled)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vignette.enabled = enabled;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VIGNETTE;
        UpdateTimeStamp();
    }
    bool VzRenderer::IsVignetteEnabled()
    {
        COMP_RENDERPATH(render_path, false);
        return render_path->viewSettings.vignette.enabled;
    }
    void VzRenderer::SetVignetteMidPoint(float midPoint)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vignette.midPoint = midPoint;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VIGNETTE;
        UpdateTimeStamp();
    }
    float VzRenderer::GetVignetteMidPoint()
    {
        COMP_RENDERPATH(render_path, 0.5f);
        return render_path->viewSettings.vignette.midPoint;
    }
    void VzRenderer::SetVignetteRoundness(float roundness)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vignette.roundness = roundness;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VIGNETTE;
        UpdateTimeStamp();
    }
    float VzRenderer::GetVignetteRoundness()
    {
        COMP_RENDERPATH(render_path, 0.5f);
        return render_path->viewSettings.vignette.roundness;
    }
    void VzRenderer::SetVignetteFeather(float feather)
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vignette.feather = feather;
        render_path->dirtyFlags |= VzRenderPath::DirtyFlags::VIGNETTE;
        UpdateTimeStamp();
    }
    float VzRenderer::GetVignetteFeather()
    {
        COMP_RENDERPATH(render_path, 0.5f);
        return render_path->viewSettings.vignette.feather;
    }
    void VzRenderer::SetVignetteColor(const float color[3])
    {
        COMP_RENDERPATH(render_path, );
        render_path->viewSettings.vignette.color.r = color[0];
        render_path->viewSettings.vignette.color.g = color[1];
        render_path->viewSettings.vignette.color.b = color[2];
    }
    void VzRenderer::GetVignetteColor(float color[3])
    {
        COMP_RENDERPATH(render_path, );
        color[0] = render_path->viewSettings.vignette.color.r;
        color[1] = render_path->viewSettings.vignette.color.g;
        color[2] = render_path->viewSettings.vignette.color.b;
    }
#pragma endregion

    void VzRenderer::SetClearOptions(const ClearOptions& clearOptions)
    {
        COMP_RENDERPATH(render_path, );
        render_path->GetRenderer()->setClearOptions((Renderer::ClearOptions&)clearOptions);
        UpdateTimeStamp();
    }

    void VzRenderer::GetClearOptions(ClearOptions& clearOptions)
    {
        COMP_RENDERPATH(render_path, );
        clearOptions = (ClearOptions&)render_path->GetRenderer()->getClearOptions();
    }

    VZRESULT VzRenderer::Render(const VID vidScene, const VID vidCam)
    {
        VzRenderPath* render_path = gEngineApp->GetRenderPath(GetVID());
        if (render_path == nullptr)
        {
            backlog::post("invalid render path", backlog::LogLevel::Error);
            return VZ_FAIL;
        }

        View* view = render_path->GetView();
        Scene* scene = gEngineApp->GetScene(vidScene);
        Camera* camera = gEngine->getCameraComponent(utils::Entity::import(vidCam));
        if (view == nullptr || scene == nullptr || camera == nullptr)
        {
            backlog::post("renderer has nullptr : " + std::string(view == nullptr ? "view " : "") + std::string(scene == nullptr ? "scene " : "") + std::string(camera == nullptr ? "camera" : ""), backlog::LogLevel::Error);
            return VZ_FAIL;
        }
        render_path->TryResizeRenderTargets();
        view->setScene(scene);
        view->setCamera(camera);
        //view->setVisibleLayers(0x4, 0x4);
        //SceneVID vid_scene = gEngineApp->GetSceneVidBelongTo(vidCam);
        //assert(vid_scene != INVALID_VID);

        if (!UTILS_HAS_THREADING)
        {
            gEngine->execute();
        }

        VzCameraRes* cam_res = gEngineApp->GetCameraRes(vidCam);
        if (cam_res->cameraControllerEnabled)
        {
            cam_res->UpdateCameraWithCM(render_path->deltaTime);
        }

        if (cam_res->FRAMECOUNT == 0)
        {
            cam_res->timer = std::chrono::high_resolution_clock::now();
        }

        // fixed time update
        if (0)
        {
            render_path->deltaTimeAccumulator += render_path->deltaTime;
            if (render_path->deltaTimeAccumulator > 10)
            {
                // application probably lost control, fixed update would take too long
                render_path->deltaTimeAccumulator = 0;
            }

            const float targetFrameRateInv = 1.0f / render_path->GetFixedTimeUpdate();
            while (render_path->deltaTimeAccumulator >= targetFrameRateInv)
            {
                //renderer->FixedUpdate();
                render_path->deltaTimeAccumulator -= targetFrameRateInv;
            }
        }

        // Update the cube distortion matrix used for frustum visualization.
        const Camera* lightmapCamera = view->getDirectionalShadowCamera();
        if (lightmapCamera)
        {
            VzSceneRes* scene_res = gEngineApp->GetSceneRes(vidScene);
            VzCube* lightmapCube = scene_res->GetLightmapCube();
            lightmapCube->mapFrustum(*gEngine, lightmapCamera);
        }
        VzCube* cameraCube = cam_res->GetCameraCube();
        if (cameraCube)
        {
            cameraCube->mapFrustum(*gEngine, camera);
        }

        ResourceLoader* resource_loader = gEngineApp->GetGltfResourceLoader();
        if (resource_loader)
        {
            resource_loader->asyncUpdateLoad();

            //static std::set<Texture*> regTexMap;
            VzAssetRes* asset_res = gEngineApp->GetAssetRes(gEngineApp->activeAsyncAsset);
            if (asset_res && resource_loader->asyncGetLoadProgress() >= 1.)
            {
                int count = 0;
                gEngineApp->activeAsyncAsset = INVALID_VID;
                filament::gltfio::FFilamentAsset* fasset = downcast(asset_res->asset);
                for (auto& it : asset_res->asyncTextures)
                {
                    VzTextureRes* tex_res = gEngineApp->GetTextureRes(it.second);
                    tex_res->texture = fasset->mTextures[it.first].texture;
                    tex_res->isAsyncLocked = false;
                }
                //backlog::post("REDUNDANT TEXTURES : " + std::to_string(count), backlog::LogLevel::Default);
                asset_res->asyncTextures.clear();
            }
        }

        std::unordered_map<AssetVID, std::unique_ptr<VzAssetRes>>& assetResMap = *gEngineApp->GetAssetResMap();

        for (auto& it : assetResMap)
        {
            VzAssetRes* asset_res = it.second.get();
            VzAsset* v_asset = gEngineApp->GetVzComponent<VzAsset>(it.first);
            assert(v_asset);
            vzm::VzAsset::Animator* animator = v_asset->GetAnimator();
            if (animator->IsPlayScene(vidScene))
            {
                animator->UpdateAnimation();
            }
        }

#pragma region Animation
        std::vector<float> weights;

        auto applyAnimation = [&](const skm::Channel& channel, float t, size_t prevIndex, size_t nextIndex) {
            const skm::Sampler* sampler = channel.sourceData;
            const skm::TimeValues& times = sampler->times;
            auto trsTransformManager = &gEngineApp->GetTrsTransformManager();
            auto transformManager = &gEngine->getTransformManager();
            auto renderableManager = &gEngine->getRenderableManager();
            TrsTransformManager::Instance trsNode = trsTransformManager->getInstance(channel.targetEntity);
            TransformManager::Instance node = transformManager->getInstance(channel.targetEntity);

            switch (channel.transformType)
            {
            case skm::Channel::SCALE:
            {
                float3 scale;
                const float3* srcVec3 = (const float3*)sampler->values.data();
                if (sampler->interpolation == skm::Sampler::CUBIC)
                {
                    float3 vert0 = srcVec3[prevIndex * 3 + 1];
                    float3 tang0 = srcVec3[prevIndex * 3 + 2];
                    float3 tang1 = srcVec3[nextIndex * 3];
                    float3 vert1 = srcVec3[nextIndex * 3 + 1];
                    scale = cubicSpline(vert0, tang0, vert1, tang1, t);
                }
                else
                {
                    scale = ((1 - t) * srcVec3[prevIndex]) + (t * srcVec3[nextIndex]);
                }
                trsTransformManager->setScale(trsNode, scale);
                break;
            }

            case skm::Channel::TRANSLATION:
            {
                float3 translation;
                const float3* srcVec3 = (const float3*)sampler->values.data();
                if (sampler->interpolation == skm::Sampler::CUBIC)
                {
                    float3 vert0 = srcVec3[prevIndex * 3 + 1];
                    float3 tang0 = srcVec3[prevIndex * 3 + 2];
                    float3 tang1 = srcVec3[nextIndex * 3];
                    float3 vert1 = srcVec3[nextIndex * 3 + 1];
                    translation = cubicSpline(vert0, tang0, vert1, tang1, t);
                }
                else
                {
                    translation = ((1 - t) * srcVec3[prevIndex]) + (t * srcVec3[nextIndex]);
                }
                trsTransformManager->setTranslation(trsNode, translation);
                break;
            }

            case skm::Channel::ROTATION:
            {
                quatf rotation;
                const quatf* srcQuat = (const quatf*)sampler->values.data();
                if (sampler->interpolation == skm::Sampler::CUBIC)
                {
                    quatf vert0 = srcQuat[prevIndex * 3 + 1];
                    quatf tang0 = srcQuat[prevIndex * 3 + 2];
                    quatf tang1 = srcQuat[nextIndex * 3];
                    quatf vert1 = srcQuat[nextIndex * 3 + 1];
                    rotation = normalize(cubicSpline(vert0, tang0, vert1, tang1, t));
                }
                else
                {
                    rotation = slerp(srcQuat[prevIndex], srcQuat[nextIndex], t);
                }
                trsTransformManager->setRotation(trsNode, rotation);
                break;
            }

            case skm::Channel::WEIGHTS:
            {
                const float* const samplerValues = sampler->values.data();
                assert(sampler->values.size() % times.size() == 0);
                const int valuesPerKeyframe = sampler->values.size() / times.size();

                if (sampler->interpolation == skm::Sampler::CUBIC)
                {
                    assert(valuesPerKeyframe % 3 == 0);
                    const int numMorphTargets = valuesPerKeyframe / 3;
                    const float* const inTangents = samplerValues;
                    const float* const splineVerts = samplerValues + numMorphTargets;
                    const float* const outTangents = samplerValues + numMorphTargets * 2;

                    weights.resize(numMorphTargets);
                    for (int comp = 0; comp < numMorphTargets; ++comp)
                    {
                        float vert0 = splineVerts[comp + prevIndex * valuesPerKeyframe];
                        float tang0 = outTangents[comp + prevIndex * valuesPerKeyframe];
                        float tang1 = inTangents[comp + nextIndex * valuesPerKeyframe];
                        float vert1 = splineVerts[comp + nextIndex * valuesPerKeyframe];
                        weights[comp] = cubicSpline(vert0, tang0, vert1, tang1, t);
                    }
                }
                else
                {
                    weights.resize(valuesPerKeyframe);
                    for (int comp = 0; comp < valuesPerKeyframe; ++comp)
                    {
                        float previous = samplerValues[comp + prevIndex * valuesPerKeyframe];
                        float current = samplerValues[comp + nextIndex * valuesPerKeyframe];
                        weights[comp] = (1 - t) * previous + t * current;
                    }
                }

                auto ci = renderableManager->getInstance(channel.targetEntity);
                renderableManager->setMorphWeights(ci, weights.data(), weights.size());
                return;
            }
            }

            transformManager->setTransform(node, trsTransformManager->getTransform(trsNode));
        };

        auto& aniResMap = gEngineApp->GetAniResMap();

        for (auto& it : aniResMap)
        {
            VzAniRes* ani_res = it.second.get();
            if (ani_res->isPlaying)
            {
                auto& time = ani_res->currentTime;
                auto& duration = ani_res->animation->duration;
                float effectiveDeltaTime = render_path->deltaTime * (ani_res->isReversing ? (-ani_res->playRate) : (ani_res->playRate));
                float newTime = time + effectiveDeltaTime;
                if (effectiveDeltaTime > 0.0f)
                {
                    if (newTime >= duration)
                    {
                        switch (ani_res->loopMode)
                        {
                        case VzAnimation::LoopMode::ONCE:
                            newTime = duration ;
                            ani_res->isPlaying = false;
                            break;
                        case VzAnimation::LoopMode::LOOP:
                            if (duration > 0.0f)
                            {
                                while (newTime >= duration)
                                {
                                    newTime -= duration;
                                }
                            }
                            else
                            {
                                newTime = 0.0f;
                            }
                            break;
                        case VzAnimation::LoopMode::PING_PONG:
                            ani_res->isReversing = !ani_res->isReversing;
                            newTime = duration - (newTime - duration);
                            break;
                        }
                    }
                }
                else
                {
                    if (newTime < 0.0f)
                    {
                        switch (ani_res->loopMode)
                        {
                        case VzAnimation::LoopMode::ONCE:
                            newTime = 0.0f;
                            ani_res->isPlaying = false;
                            break;
                        case VzAnimation::LoopMode::LOOP:
                            if (duration > 0.0f)
                            {
                                while (newTime < 0.0f)
                                {
                                    newTime += duration;
                                }
                            }
                            else
                            {
                                newTime = 0.0f;
                            }
                            break;
                        case VzAnimation::LoopMode::PING_PONG:
                            ani_res->isReversing = !ani_res->isReversing;
                            newTime = -newTime;
                            break;
                        }
                    }
                }
                time = newTime;
                const skm::Animation& anim = *ani_res->animation;
                TransformManager& transformManager = gEngine->getTransformManager();
                transformManager.openLocalTransformTransaction();
                for (const auto& channel : anim.channels)
                {
                    const skm::Sampler* sampler = channel.sourceData;
                    if (sampler->times.size() < 2)
                    {
                        continue;
                    }

                    const skm::TimeValues& times = sampler->times;

                    // Find the first keyframe after the given time, or the keyframe that matches it exactly.
                    skm::TimeValues::const_iterator iter = times.lower_bound(time);

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
                        skm::TimeValues::const_iterator prev = iter;
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

                    if (sampler->interpolation == skm::Sampler::STEP)
                    {
                        t = 0.0f;
                    }

                    applyAnimation(channel, t, prevIndex, nextIndex);
                }
                transformManager.commitLocalTransformTransaction();
            }
        }
#pragma endregion

#pragma region Skeleton
        auto& skelResMap = gEngineApp->GetSkeletonResMap();

        for (auto& it : skelResMap)
        {
            VzSkeleton* skel = gEngineApp->GetVzComponent<VzSkeleton>(it.first);
            skel->UpdateBoneMatrices();
        }
#pragma endregion

        Renderer* renderer = render_path->GetRenderer();

        auto& tcm = gEngine->getTransformManager();
        tcm.commitLocalTransformTransaction();

        double3 v = camera->getForwardVector();
        double3 u = camera->getUpVector();
        //auto ToString = [](double3 v) -> std::string
        //    {
        //        using namespace std;
        //        return "(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ")";
        //    };
        //static size_t debug_count = 0;
        //if (debug_count++ % 50 == 0)
        //{
        //    backlog::post("view : " + ToString(v), backlog::LogLevel::Default);
        //    backlog::post("up   : " + ToString(u), backlog::LogLevel::Default);
        //}

        std::map<Entity, mat4f> restore_billboard_tr;
        scene->forEach([&tcm, &restore_billboard_tr, &u, &v](Entity ett) {
            VID vid = ett.getId();

            VzSceneComp* comp = gEngineApp->GetVzComponent<VzSceneComp>(vid);
            if (comp && comp->IsMatrixAutoUpdate())
            {
                comp->UpdateMatrix();
            }

            VzActorRes* actor_res = gEngineApp->GetActorRes(vid);
            if (actor_res && actor_res->isBillboard)
            {
                auto ti = tcm.getInstance(ett);
                mat4f os2parent = tcm.getTransform(ti); // local
                restore_billboard_tr[ett] = os2parent;

                mat4 os2ws = (mat4)tcm.getWorldTransform(ti);
                mat4 parent2ws = os2ws * inverse(os2parent); // fixed
                double4 p_ws_h = os2ws * double4(0, 0, 0, 1);
                double3 p_ws = p_ws_h.xyz / p_ws_h.w; // fixed

                mat4 os2ws_new = mat4::lookTo(v, p_ws, u);
                mat4 os2parent_new = inverse(parent2ws) * os2ws_new;

                tcm.setTransform(ti, os2parent_new);
            }
        });

        filament::Texture* fogColorTexture = gEngineApp->GetSceneRes(vidScene)->GetIBL()->getFogTexture();
        render_path->viewSettings.fogSettings.fogColorTexture = fogColorTexture;
        render_path->ApplySettings();

        // 1. main rendering
        view->setVisibleLayers(0x3, 0x1);
        view->setPostProcessingEnabled(true);
        view->setRenderTarget(render_path->GetOffscreenRT());
        renderer->renderStandaloneView(view);

        // 2. gui rendering wo/ postprocessing
        View* view_gui = render_path->GetGuiView();
        //scene->setSkybox(nullptr);
        view_gui->setScene(scene);
        view_gui->setCamera(camera);
        view_gui->setPostProcessingEnabled(false);
        view_gui->setVisibleLayers(0x3, 0x2);
        view_gui->setRenderTarget(render_path->GetOffscreenGuiRT());

        Renderer::ClearOptions restore_clear_options = renderer->getClearOptions();
        Renderer::ClearOptions clear_options;
        clear_options.clearColor = float4{ 0, 0, 0, 0 };
        clear_options.clear = true;
        clear_options.discard = true;
        renderer->setClearOptions(clear_options);

        renderer->renderStandaloneView(view_gui);

        // 3. compositor
        CompositorQuad* compositor = gEngineApp->GetCompositorQuad();
        View* view_compositor = render_path->GetCompositorView();
        view_compositor->setCamera(compositor->GetQaudCamera());
        view_compositor->setScene(compositor->GetQuadScene());
        view_compositor->setPostProcessingEnabled(false);
        view_compositor->setRenderTarget(nullptr);

        MaterialInstance* quad_mi = compositor->GetMaterial();

        quad_mi->setParameter("mainTexture", render_path->GetOffscreenRT()->getTexture(RenderTarget::AttachmentPoint::COLOR), compositor->sampler);
        quad_mi->setParameter("guiTexture", render_path->GetOffscreenGuiRT()->getTexture(RenderTarget::AttachmentPoint::COLOR), compositor->sampler);

        //Texture* mainDepth = render_path->GetOffscreenRT()->getTexture(RenderTarget::AttachmentPoint::DEPTH);
        //Texture* guiDepth = render_path->GetOffscreenGuiRT()->getTexture(RenderTarget::AttachmentPoint::DEPTH);
        //quad_mi->setParameter("mainDepth", render_path->GetOffscreenRT()->getTexture(RenderTarget::AttachmentPoint::DEPTH), compositor->samplerPoint);
        //quad_mi->setParameter("guiDepth", render_path->GetOffscreenGuiRT()->getTexture(RenderTarget::AttachmentPoint::DEPTH), compositor->samplerPoint);
        //quad_mi->setParameter("baseColorMap", render_path->GetOffscreenRT()->getTexture(RenderTarget::AttachmentPoint::COLOR), sampler);

        clear_options.clear = false;
        clear_options.discard = true;
        renderer->setClearOptions(clear_options);

        Fence::waitAndDestroy(gEngine->createFence());

        filament::SwapChain* sc = render_path->GetSwapChain();
        if (renderer->beginFrame(sc))
        {
            renderer->render(view_compositor);
            renderer->endFrame();
        }

        renderer->setClearOptions(restore_clear_options);

        for (auto& it : restore_billboard_tr)
        {
            auto ti = tcm.getInstance(it.first);
            tcm.setTransform(ti, it.second);
        }

        if (gEngine->getBackend() == Backend::OPENGL)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        TimeStamp timer2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(timer2 - cam_res->timer);
        cam_res->timer = timer2;
        render_path->deltaTime = (float)time_span.count();
        render_path->FRAMECOUNT++;

        return VZ_OK;
    }
}
