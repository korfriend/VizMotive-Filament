#include "VzCompositor.h"

#include "../FIncludes.h"
#include "../VzEngineApp.h"
#include "../VzRenderPath.h"
#include "VzCompositor.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

template <>
struct utils::EnableBitMaskOperators<CompositorLayer::DirtyFlags>
    : public std::true_type {};
namespace vzm {
struct CompositorLayer::Impl {
  filament::View* view_ = nullptr;
  ViewSettings viewSettings_;
};

CompositorLayer::CompositorLayer(VID vidScene, VID vidCamera,
                                 vzm::VzBaseActor::VISIBLE_LAYER layerMask)
    : pImpl(new Impl()) {
  pImpl->view_ = gEngine->createView();
  vidScene_ = vidScene;
  vidCamera_ = vidCamera;
  layerMask_ = layerMask;
}

CompositorLayer::~CompositorLayer() {
  gEngine->destroy(pImpl->view_);
  delete pImpl;
}

void CompositorLayer::SetViewport(const uint32_t x, const uint32_t y,
                                  const uint32_t w, const uint32_t h) {
  pImpl->view_->setViewport(filament::Viewport(x, y, w, h));
}

void CompositorLayer::GetViewport(uint32_t* x, uint32_t* y, uint32_t* w,
                                  uint32_t* h) {
  filament::Viewport viewport = pImpl->view_->getViewport();
  if (x) *x = viewport.left;
  if (y) *y = viewport.bottom;
  if (w) *w = viewport.width;
  if (h) *h = viewport.height;
}

void CompositorLayer::SetScene(VID vidScene) { vidScene_ = vidScene; }

VID CompositorLayer::GetScene() const { return vidScene_; }

void CompositorLayer::SetCamera(VID vidCamera) { vidCamera_ = vidCamera; }

VID CompositorLayer::GetCamera() const { return vidCamera_; }

void CompositorLayer::ApplyViewSettingsFrom(CompositorLayer* other) {
  pImpl->viewSettings_ = other->pImpl->viewSettings_;
  dirtyFlags_ = DirtyFlags::ALL;
}

void CompositorLayer::ApplySettings() {
  if (any(dirtyFlags_ & DirtyFlags::ANTI_ALIASING))
    pImpl->view_->setAntiAliasing(pImpl->viewSettings_.antiAliasing);
  if (any(dirtyFlags_ & DirtyFlags::TAA))
    pImpl->view_->setTemporalAntiAliasingOptions(pImpl->viewSettings_.taa);
  if (any(dirtyFlags_ & DirtyFlags::MSAA))
    pImpl->view_->setMultiSampleAntiAliasingOptions(pImpl->viewSettings_.msaa);
  if (any(dirtyFlags_ & DirtyFlags::DSR))
    pImpl->view_->setDynamicResolutionOptions(pImpl->viewSettings_.dsr);
  if (any(dirtyFlags_ & DirtyFlags::SSAO))
    pImpl->view_->setAmbientOcclusionOptions(pImpl->viewSettings_.ssao);
  if (any(dirtyFlags_ & DirtyFlags::SCREEN_SPACE_REFLECTIONS))
    pImpl->view_->setScreenSpaceReflectionsOptions(
        pImpl->viewSettings_.screenSpaceReflections);
  if (any(dirtyFlags_ & DirtyFlags::BLOOM))
    pImpl->view_->setBloomOptions(pImpl->viewSettings_.bloom);
  if (any(dirtyFlags_ & DirtyFlags::FOG))
    pImpl->view_->setFogOptions(pImpl->viewSettings_.fog);
  if (any(dirtyFlags_ & DirtyFlags::DOF))
    pImpl->view_->setDepthOfFieldOptions(pImpl->viewSettings_.dof);
  if (any(dirtyFlags_ & DirtyFlags::VIGNETTE))
    pImpl->view_->setVignetteOptions(pImpl->viewSettings_.vignette);
  if (any(dirtyFlags_ & DirtyFlags::DITHERING))
    pImpl->view_->setDithering(pImpl->viewSettings_.dithering);
  if (any(dirtyFlags_ & DirtyFlags::RENDER_QUALITY))
    pImpl->view_->setRenderQuality(pImpl->viewSettings_.renderQuality);
  if (any(dirtyFlags_ & DirtyFlags::DYNAMIC_LIGHTING))
    pImpl->view_->setDynamicLightingOptions(
        pImpl->viewSettings_.dynamicLighting.zLightNear,
        pImpl->viewSettings_.dynamicLighting.zLightFar);
  if (any(dirtyFlags_ & DirtyFlags::SHADOW_TYPE))
    pImpl->view_->setShadowType(pImpl->viewSettings_.shadowType);
  if (any(dirtyFlags_ & DirtyFlags::VSM_SHADOW_OPTIONS))
    pImpl->view_->setVsmShadowOptions(pImpl->viewSettings_.vsmShadowOptions);
  if (any(dirtyFlags_ & DirtyFlags::SOFT_SHADOW_OPTIONS))
    pImpl->view_->setSoftShadowOptions(pImpl->viewSettings_.softShadowOptions);
  if (any(dirtyFlags_ & DirtyFlags::GUARD_BAND))
    pImpl->view_->setGuardBandOptions(pImpl->viewSettings_.guardBand);
  if (any(dirtyFlags_ & DirtyFlags::STEREOSCOPIC_OPTIONS))
    pImpl->view_->setStereoscopicOptions(
        pImpl->viewSettings_.stereoscopicOptions);
  if (any(dirtyFlags_ & DirtyFlags::POST_PROCESSING_ENABLED))
    pImpl->view_->setPostProcessingEnabled(
        pImpl->viewSettings_.postProcessingEnabled);
  dirtyFlags_ = DirtyFlags::NONE;
}

void CompositorLayer::SetVisibleLayerMask(const uint32_t layerBits,
                                          const uint8_t maskBits) {
  pImpl->view_->setVisibleLayers(layerBits, maskBits);
}
#pragma region View
void CompositorLayer::SetPostProcessingEnabled(bool enabled) {
  pImpl->viewSettings_.postProcessingEnabled = enabled;
  dirtyFlags_ |= DirtyFlags::POST_PROCESSING_ENABLED;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsPostProcessingEnabled() {
  return pImpl->viewSettings_.postProcessingEnabled;
}
void CompositorLayer::SetDitheringEnabled(bool enabled) {
  pImpl->viewSettings_.dithering =
      enabled ? Dithering::TEMPORAL : Dithering::NONE;
  dirtyFlags_ |= DirtyFlags::DITHERING;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDitheringEnabled() {
  return pImpl->viewSettings_.dithering == Dithering::TEMPORAL;
}
void CompositorLayer::SetBloomEnabled(bool enabled) {
  pImpl->viewSettings_.bloom.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsBloomEnabled() {
  return pImpl->viewSettings_.bloom.enabled;
}
void CompositorLayer::SetTaaEnabled(bool enabled) {
  pImpl->viewSettings_.taa.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaEnabled() {
  return pImpl->viewSettings_.taa.enabled;
}
void CompositorLayer::SetFxaaEnabled(bool enabled) {
  pImpl->viewSettings_.antiAliasing =
      enabled ? AntiAliasing::FXAA : AntiAliasing::NONE;
  dirtyFlags_ |= DirtyFlags::ANTI_ALIASING;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsFxaaEnabled() {
  return pImpl->viewSettings_.antiAliasing == AntiAliasing::FXAA;
}
void CompositorLayer::SetMsaaEnabled(bool enabled) {
  pImpl->viewSettings_.msaa.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::MSAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsMsaaEnabled() {
  return pImpl->viewSettings_.msaa.enabled;
}
void CompositorLayer::SetMsaaSampleCount(int samples) {
  pImpl->viewSettings_.msaa.sampleCount = samples;
  dirtyFlags_ |= DirtyFlags::MSAA;
  // UpdateTimeStamp();
}
int CompositorLayer::GetMsaaSampleCount() {
  return pImpl->viewSettings_.msaa.sampleCount;
}
void CompositorLayer::SetMsaaCustomResolve(bool customResolve) {
  pImpl->viewSettings_.msaa.customResolve = customResolve;
  dirtyFlags_ |= DirtyFlags::MSAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsMsaaCustomResolve() {
  return pImpl->viewSettings_.msaa.customResolve;
}
void CompositorLayer::SetSsaoEnabled(bool enabled) {
  pImpl->viewSettings_.ssao.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsSsaoEnabled() {
  return pImpl->viewSettings_.ssao.enabled;
}
void CompositorLayer::SetScreenSpaceReflectionsEnabled(bool enabled) {
  pImpl->viewSettings_.screenSpaceReflections.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::SCREEN_SPACE_REFLECTIONS;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsScreenSpaceReflectionsEnabled() {
  return pImpl->viewSettings_.screenSpaceReflections.enabled;
}
void CompositorLayer::SetGuardBandEnabled(bool enabled) {
  pImpl->viewSettings_.guardBand.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::GUARD_BAND;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsGuardBandEnabled() {
  return pImpl->viewSettings_.guardBand.enabled;
}
#pragma endregion
#pragma region Bloom Options
void CompositorLayer::SetBloomStrength(float strength) {
  pImpl->viewSettings_.bloom.strength = strength;
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
float CompositorLayer::GetBloomStrength() {
  return pImpl->viewSettings_.bloom.strength;
}
void CompositorLayer::SetBloomThreshold(bool threshold) {
  pImpl->viewSettings_.bloom.threshold = threshold;
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsBloomThreshold() {
  return pImpl->viewSettings_.bloom.threshold;
}
void CompositorLayer::SetBloomLevels(int levels) {
  pImpl->viewSettings_.bloom.levels = levels;
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
int CompositorLayer::GetBloomLevels() {
  return pImpl->viewSettings_.bloom.levels;
}
void CompositorLayer::SetBloomQuality(int quality) {
  pImpl->viewSettings_.bloom.quality = (QualityLevel)std::clamp(quality, 0, 3);
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
int CompositorLayer::GetBloomQuality() {
  return (int)pImpl->viewSettings_.bloom.quality;
}
void CompositorLayer::SetBloomLensFlare(bool lensFlare) {
  pImpl->viewSettings_.bloom.lensFlare = lensFlare;
  dirtyFlags_ |= DirtyFlags::BLOOM;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsBloomLensFlare() {
  return pImpl->viewSettings_.bloom.lensFlare;
}
#pragma endregion
#pragma region TAA Options
void CompositorLayer::SetTaaUpscaling(bool upscaling) {
  pImpl->viewSettings_.taa.upscaling = upscaling;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaUpscaling() {
  return pImpl->viewSettings_.taa.upscaling;
}
void CompositorLayer::SetTaaHistoryReprojection(bool historyReprojection) {
  pImpl->viewSettings_.taa.historyReprojection = historyReprojection;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaHistoryReprojection() {
  return pImpl->viewSettings_.taa.historyReprojection;
}
void CompositorLayer::SetTaaFeedback(float feedback) {
  pImpl->viewSettings_.taa.feedback = feedback;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
float CompositorLayer::GetTaaFeedback() {
  return pImpl->viewSettings_.taa.feedback;
}
void CompositorLayer::SetTaaFilterHistory(bool filterHistory) {
  pImpl->viewSettings_.taa.filterHistory = filterHistory;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaFilterHistory() {
  return pImpl->viewSettings_.taa.filterHistory;
}
void CompositorLayer::SetTaaFilterInput(bool filterInput) {
  pImpl->viewSettings_.taa.filterInput = filterInput;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaFilterInput() {
  return pImpl->viewSettings_.taa.filterInput;
}
void CompositorLayer::SetTaaFilterWidth(float filterWidth) {
  pImpl->viewSettings_.taa.filterWidth = filterWidth;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
float CompositorLayer::GetTaaFilterWidth() {
  return pImpl->viewSettings_.taa.filterWidth;
}
void CompositorLayer::SetTaaLodBias(float lodBias) {
  pImpl->viewSettings_.taa.lodBias = lodBias;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
float CompositorLayer::GetTaaLodBias() {
  return pImpl->viewSettings_.taa.lodBias;
}
void CompositorLayer::SetTaaUseYCoCg(bool useYCoCg) {
  pImpl->viewSettings_.taa.useYCoCg = useYCoCg;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaUseYCoCg() {
  return pImpl->viewSettings_.taa.useYCoCg;
}
void CompositorLayer::SetTaaPreventFlickering(bool preventFlickering) {
  pImpl->viewSettings_.taa.preventFlickering = preventFlickering;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsTaaPreventFlickering() {
  return pImpl->viewSettings_.taa.preventFlickering;
}
void CompositorLayer::SetTaaJitterPattern(JitterPattern pattern) {
  pImpl->viewSettings_.taa.jitterPattern =
      (TemporalAntiAliasingOptions::JitterPattern)pattern;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
CompositorLayer::JitterPattern CompositorLayer::GetTaaJitterPattern() {
  return (JitterPattern)pImpl->viewSettings_.taa.jitterPattern;
}
void CompositorLayer::SetTaaBoxClipping(BoxClipping boxClipping) {
  pImpl->viewSettings_.taa.boxClipping =
      (TemporalAntiAliasingOptions::BoxClipping)boxClipping;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
CompositorLayer::BoxClipping CompositorLayer::GetTaaBoxClipping() {
  return (BoxClipping)pImpl->viewSettings_.taa.boxClipping;
}
void CompositorLayer::SetTaaBoxType(BoxType boxType) {
  pImpl->viewSettings_.taa.boxType =
      (TemporalAntiAliasingOptions::BoxType)boxType;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
CompositorLayer::BoxType CompositorLayer::GetTaaBoxType() {
  return (BoxType)pImpl->viewSettings_.taa.boxType;
}
void CompositorLayer::SetTaaVarianceGamma(float varianceGamma) {
  pImpl->viewSettings_.taa.varianceGamma = varianceGamma;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
float CompositorLayer::GetTaaVarianceGamma() {
  return pImpl->viewSettings_.taa.varianceGamma;
}
void CompositorLayer::SetTaaSharpness(float sharpness) {
  pImpl->viewSettings_.taa.sharpness = sharpness;
  dirtyFlags_ |= DirtyFlags::TAA;
  // UpdateTimeStamp();
}
float CompositorLayer::GetTaaSharpness() {
  return pImpl->viewSettings_.taa.sharpness;
}
#pragma endregion
#pragma region SSAO Options
void CompositorLayer::SetSsaoQuality(int quality) {
  pImpl->viewSettings_.ssao.quality = (QualityLevel)std::clamp(quality, 0, 3);
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
int CompositorLayer::GetSsaoQuality() {
  return (int)pImpl->viewSettings_.ssao.quality;
}
void CompositorLayer::SetSsaoLowPassFilter(int lowPassFilter) {
  pImpl->viewSettings_.ssao.lowPassFilter =
      (QualityLevel)std::clamp(lowPassFilter, 0, 2);
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
int CompositorLayer::GetSsaoLowPassFilter() {
  return (int)pImpl->viewSettings_.ssao.lowPassFilter;
}
void CompositorLayer::SetSsaoBentNormals(bool bentNormals) {
  pImpl->viewSettings_.ssao.bentNormals = bentNormals;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsSsaoBentNormals() {
  return pImpl->viewSettings_.ssao.bentNormals;
}
void CompositorLayer::SetSsaoUpsampling(bool upsampling) {
  pImpl->viewSettings_.ssao.upsampling =
      upsampling ? QualityLevel::HIGH : QualityLevel::LOW;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsSsaoUpsampling() {
  return pImpl->viewSettings_.ssao.upsampling != QualityLevel::LOW;
}
void CompositorLayer::SetSsaoMinHorizonAngleRad(float minHorizonAngleRad) {
  pImpl->viewSettings_.ssao.minHorizonAngleRad = minHorizonAngleRad;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoMinHorizonAngleRad() {
  return pImpl->viewSettings_.ssao.minHorizonAngleRad;
}
void CompositorLayer::SetSsaoBilateralThreshold(float bilateralThreshold) {
  pImpl->viewSettings_.ssao.bilateralThreshold = bilateralThreshold;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoBilateralThreshold() {
  return pImpl->viewSettings_.ssao.bilateralThreshold;
}
void CompositorLayer::SetSsaoHalfResolution(bool halfResolution) {
  pImpl->viewSettings_.ssao.resolution = halfResolution ? 0.5f : 1.0f;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsSsaoHalfResolution() {
  return pImpl->viewSettings_.ssao.resolution != 1.0f;
}

#pragma region Dominant Light Shadows (experimental)
void CompositorLayer::SetSsaoSsctEnabled(bool enabled) {
  pImpl->viewSettings_.ssao.ssct.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsSsaoSsctEnabled() {
  return pImpl->viewSettings_.ssao.ssct.enabled;
}
void CompositorLayer::SetSsaoSsctLightConeRad(float lightConeRad) {
  pImpl->viewSettings_.ssao.ssct.lightConeRad = lightConeRad;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctLightConeRad() {
  return pImpl->viewSettings_.ssao.ssct.lightConeRad;
}
void CompositorLayer::SetSsaoSsctShadowDistance(float shadowDistance) {
  pImpl->viewSettings_.ssao.ssct.shadowDistance = shadowDistance;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctShadowDistance() {
  return pImpl->viewSettings_.ssao.ssct.shadowDistance;
}
void CompositorLayer::SetSsaoSsctContactDistanceMax(float contactDistanceMax) {
  pImpl->viewSettings_.ssao.ssct.contactDistanceMax = contactDistanceMax;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctContactDistanceMax() {
  return pImpl->viewSettings_.ssao.ssct.contactDistanceMax;
}
void CompositorLayer::SetSsaoSsctIntensity(float intensity) {
  pImpl->viewSettings_.ssao.ssct.intensity = intensity;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctIntensity() {
  return pImpl->viewSettings_.ssao.ssct.intensity;
}
void CompositorLayer::SetSsaoSsctDepthBias(float depthBias) {
  pImpl->viewSettings_.ssao.ssct.depthBias = depthBias;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctDepthBias() {
  return pImpl->viewSettings_.ssao.ssct.depthBias;
}
void CompositorLayer::SetSsaoSsctDepthSlopeBias(float depthSlopeBias) {
  pImpl->viewSettings_.ssao.ssct.depthSlopeBias = depthSlopeBias;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSsaoSsctDepthSlopeBias() {
  return pImpl->viewSettings_.ssao.ssct.depthSlopeBias;
}
void CompositorLayer::SetSsaoSsctSampleCount(int sampleCount) {
  pImpl->viewSettings_.ssao.ssct.sampleCount = (uint8_t)sampleCount;
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
int CompositorLayer::GetSsaoSsctSampleCount() {
  return pImpl->viewSettings_.ssao.ssct.sampleCount;
}
void CompositorLayer::SetSsaoSsctLightDirection(const float lightDirection[3]) {
  pImpl->viewSettings_.ssao.ssct.lightDirection.x = lightDirection[0];
  pImpl->viewSettings_.ssao.ssct.lightDirection.y = lightDirection[1];
  pImpl->viewSettings_.ssao.ssct.lightDirection.z = lightDirection[2];
  dirtyFlags_ |= DirtyFlags::SSAO;
  // UpdateTimeStamp();
}
void CompositorLayer::GetSsaoSsctLightDirection(float lightDirection[3]) {
  lightDirection[0] = pImpl->viewSettings_.ssao.ssct.lightDirection.x;
  lightDirection[1] = pImpl->viewSettings_.ssao.ssct.lightDirection.y;
  lightDirection[2] = pImpl->viewSettings_.ssao.ssct.lightDirection.z;
}
#pragma endregion

#pragma endregion
#pragma region Screen-space reflections Options
void CompositorLayer::SetScreenSpaceReflectionsThickness(float thickness) {
  pImpl->viewSettings_.screenSpaceReflections.thickness = thickness;
  dirtyFlags_ |= DirtyFlags::SCREEN_SPACE_REFLECTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetScreenSpaceReflectionsThickness() {
  return pImpl->viewSettings_.screenSpaceReflections.thickness;
}
void CompositorLayer::SetScreenSpaceReflectionsBias(float bias) {
  pImpl->viewSettings_.screenSpaceReflections.bias = bias;
  dirtyFlags_ |= DirtyFlags::SCREEN_SPACE_REFLECTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetScreenSpaceReflectionsBias() {
  return pImpl->viewSettings_.screenSpaceReflections.bias;
}
void CompositorLayer::SetScreenSpaceReflectionsMaxDistance(float maxDistance) {
  pImpl->viewSettings_.screenSpaceReflections.maxDistance = maxDistance;
  dirtyFlags_ |= DirtyFlags::SCREEN_SPACE_REFLECTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetScreenSpaceReflectionsMaxDistance() {
  return pImpl->viewSettings_.screenSpaceReflections.maxDistance;
}
void CompositorLayer::SetScreenSpaceReflectionsStride(float stride) {
  pImpl->viewSettings_.screenSpaceReflections.stride = stride;
  dirtyFlags_ |= DirtyFlags::SCREEN_SPACE_REFLECTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetScreenSpaceReflectionsStride() {
  return pImpl->viewSettings_.screenSpaceReflections.stride;
}
#pragma endregion
#pragma region Dynamic Resolution
void CompositorLayer::SetDynamicResoultionEnabled(bool enabled) {
  pImpl->viewSettings_.dsr.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDynamicResoultionEnabled() {
  return pImpl->viewSettings_.dsr.enabled;
}
void CompositorLayer::SetDynamicResoultionHomogeneousScaling(
    bool homogeneousScaling) {
  pImpl->viewSettings_.dsr.homogeneousScaling = homogeneousScaling;
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDynamicResoultionHomogeneousScaling() {
  return pImpl->viewSettings_.dsr.homogeneousScaling;
}
void CompositorLayer::SetDynamicResoultionMinScale(float minScale) {
  pImpl->viewSettings_.dsr.minScale = minScale;
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
float CompositorLayer::GetDynamicResoultionMinScale() {
  return pImpl->viewSettings_.dsr.minScale.x;
}
void CompositorLayer::SetDynamicResoultionMaxScale(float maxScale) {
  pImpl->viewSettings_.dsr.maxScale = maxScale;
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
float CompositorLayer::GetDynamicResoultionMaxScale() {
  return pImpl->viewSettings_.dsr.maxScale.x;
}
void CompositorLayer::SetDynamicResoultionQuality(int quality) {
  pImpl->viewSettings_.dsr.quality = (QualityLevel)std::clamp(quality, 0, 3);
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
int CompositorLayer::GetDynamicResoultionQuality() {
  return (int)pImpl->viewSettings_.dsr.quality;
}
void CompositorLayer::SetDynamicResoultionSharpness(float sharpness) {
  pImpl->viewSettings_.dsr.sharpness = sharpness;
  dirtyFlags_ |= DirtyFlags::DSR;
  // UpdateTimeStamp();
}
float CompositorLayer::GetDynamicResoultionSharpness() {
  return pImpl->viewSettings_.dsr.sharpness;
}
#pragma endregion
#pragma region Shadows
void CompositorLayer::SetShadowType(ShadowType shadowType) {
  pImpl->viewSettings_.shadowType = (filament::ShadowType)shadowType;
  dirtyFlags_ |= DirtyFlags::VSM_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
CompositorLayer::ShadowType CompositorLayer::GetShadowType() {
  return (ShadowType)pImpl->viewSettings_.shadowType;
}
void CompositorLayer::SetVsmHighPrecision(bool highPrecision) {
  pImpl->viewSettings_.vsmShadowOptions.highPrecision = highPrecision;
  dirtyFlags_ |= DirtyFlags::VSM_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsVsmHighPrecision() {
  return pImpl->viewSettings_.vsmShadowOptions.highPrecision;
}
void CompositorLayer::SetVsmMsaaSamples(int msaaSamples) {
  pImpl->viewSettings_.vsmShadowOptions.msaaSamples = msaaSamples;
  dirtyFlags_ |= DirtyFlags::VSM_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
int CompositorLayer::GetVsmMsaaSamples() {
  return pImpl->viewSettings_.vsmShadowOptions.msaaSamples;
}
void CompositorLayer::SetVsmAnisotropy(int anisotropy) {
  pImpl->viewSettings_.vsmShadowOptions.anisotropy = anisotropy;
  dirtyFlags_ |= DirtyFlags::VSM_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
int CompositorLayer::GetVsmAnisotropy() {
  return pImpl->viewSettings_.vsmShadowOptions.anisotropy;
}
void CompositorLayer::SetVsmMipmapping(bool mipmapping) {
  pImpl->viewSettings_.vsmShadowOptions.mipmapping = mipmapping;
  dirtyFlags_ |= DirtyFlags::VSM_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsVsmMipmapping() {
  return pImpl->viewSettings_.vsmShadowOptions.mipmapping;
}
void CompositorLayer::SetSoftShadowPenumbraScale(float penumbraScale) {
  pImpl->viewSettings_.softShadowOptions.penumbraScale = penumbraScale;
  dirtyFlags_ |= DirtyFlags::SOFT_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSoftShadowPenumbraScale() {
  return pImpl->viewSettings_.softShadowOptions.penumbraScale;
}
void CompositorLayer::SetSoftShadowPenumbraRatioScale(
    float penumbraRatioScale) {
  pImpl->viewSettings_.softShadowOptions.penumbraRatioScale =
      penumbraRatioScale;
  dirtyFlags_ |= DirtyFlags::SOFT_SHADOW_OPTIONS;
  // UpdateTimeStamp();
}
float CompositorLayer::GetSoftShadowPenumbraRatioScale() {
  return pImpl->viewSettings_.softShadowOptions.penumbraRatioScale;
}
#pragma endregion
#pragma region Fog
void CompositorLayer::SetFogEnabled(bool enabled) {
  pImpl->viewSettings_.fog.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsFogEnabled() {
  return pImpl->viewSettings_.fog.enabled;
}
void CompositorLayer::SetFogDistance(float distance) {
  pImpl->viewSettings_.fog.distance = distance;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogDistance() {
  return pImpl->viewSettings_.fog.distance;
}
void CompositorLayer::SetFogDensity(float density) {
  pImpl->viewSettings_.fog.density = density;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogDensity() {
  return pImpl->viewSettings_.fog.density;
}
void CompositorLayer::SetFogHeight(float height) {
  pImpl->viewSettings_.fog.height = height;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogHeight() {
  return pImpl->viewSettings_.fog.height;
}
void CompositorLayer::SetFogHeightFalloff(float heightFalloff) {
  pImpl->viewSettings_.fog.heightFalloff = heightFalloff;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogHeightFalloff() {
  return pImpl->viewSettings_.fog.heightFalloff;
}
void CompositorLayer::SetFogInScatteringStart(float inScatteringStart) {
  pImpl->viewSettings_.fog.inScatteringStart = inScatteringStart;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogInScatteringStart() {
  return pImpl->viewSettings_.fog.inScatteringStart;
}
void CompositorLayer::SetFogInScatteringSize(float inScatteringSize) {
  pImpl->viewSettings_.fog.inScatteringSize = inScatteringSize;
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
float CompositorLayer::GetFogInScatteringSize() {
  return pImpl->viewSettings_.fog.inScatteringSize;
}
void CompositorLayer::SetFogExcludeSkybox(bool excludeSkybox) {
  pImpl->viewSettings_.fog.cutOffDistance =
      excludeSkybox ? 1e6f : std::numeric_limits<float>::infinity();
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsFogExcludeSkybox() {
  return !std::isinf(pImpl->viewSettings_.fog.cutOffDistance);
}
void CompositorLayer::SetFogColorSource(FogColorSource fogColorSource) {
  switch (fogColorSource) {
    case FogColorSource::CONSTANT:
      pImpl->viewSettings_.fog.skyColor = nullptr;
      pImpl->viewSettings_.fog.fogColorFromIbl = false;
      break;
    case FogColorSource::IBL:
      pImpl->viewSettings_.fog.skyColor = nullptr;
      pImpl->viewSettings_.fog.fogColorFromIbl = true;
      break;
    case FogColorSource::SKYBOX:
      pImpl->viewSettings_.fog.skyColor =
          pImpl->viewSettings_.fogSettings.fogColorTexture;
      pImpl->viewSettings_.fog.fogColorFromIbl = false;
      break;
  }
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
CompositorLayer::FogColorSource CompositorLayer::GetFogColorSource() {
  FogColorSource fogColorSource = FogColorSource::CONSTANT;
  if (pImpl->viewSettings_.fog.skyColor) {
    fogColorSource = FogColorSource::SKYBOX;
  } else if (pImpl->viewSettings_.fog.fogColorFromIbl) {
    fogColorSource = FogColorSource::IBL;
  }
  return fogColorSource;
}
void CompositorLayer::SetFogColor(const float color[3]) {
  pImpl->viewSettings_.fog.color.r = color[0];
  pImpl->viewSettings_.fog.color.g = color[1];
  pImpl->viewSettings_.fog.color.b = color[2];
  dirtyFlags_ |= DirtyFlags::FOG;
  // UpdateTimeStamp();
}
void CompositorLayer::GetFogColor(float color[3]) {
  color[0] = pImpl->viewSettings_.fog.color.r;
  color[1] = pImpl->viewSettings_.fog.color.g;
  color[2] = pImpl->viewSettings_.fog.color.b;
}
#pragma endregion
#pragma region DoF
void CompositorLayer::SetDofEnabled(bool enabled) {
  pImpl->viewSettings_.dof.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDofEnabled() {
  return pImpl->viewSettings_.dof.enabled;
}
void CompositorLayer::SetDofCocScale(float cocScale) {
  pImpl->viewSettings_.dof.cocScale = cocScale;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
float CompositorLayer::GetDofCocScale() {
  return pImpl->viewSettings_.dof.cocScale;
}
void CompositorLayer::SetDofCocAspectRatio(float cocAspectRatio) {
  pImpl->viewSettings_.dof.cocAspectRatio = cocAspectRatio;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
float CompositorLayer::GetDofCocAspectRatio() {
  return pImpl->viewSettings_.dof.cocAspectRatio;
}
void CompositorLayer::SetDofRingCount(int dofRingCount) {
  pImpl->viewSettings_.dof.backgroundRingCount = (uint8_t)dofRingCount;
  pImpl->viewSettings_.dof.foregroundRingCount = (uint8_t)dofRingCount;
  pImpl->viewSettings_.dof.fastGatherRingCount = (uint8_t)dofRingCount;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
int CompositorLayer::GetDofRingCount() {
  return pImpl->viewSettings_.dof.fastGatherRingCount;
}
void CompositorLayer::SetDofMaxCoc(int maxCoc) {
  pImpl->viewSettings_.dof.maxForegroundCOC = (uint16_t)maxCoc;
  pImpl->viewSettings_.dof.maxBackgroundCOC = (uint16_t)maxCoc;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
int CompositorLayer::GetDofMaxCoc() {
  return pImpl->viewSettings_.dof.maxForegroundCOC;
}
void CompositorLayer::SetDofNativeResolution(bool nativeResolution) {
  pImpl->viewSettings_.dof.nativeResolution = nativeResolution;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDofNativeResolution() {
  return pImpl->viewSettings_.dof.nativeResolution;
}
void CompositorLayer::SetDofMedian(bool dofMedian) {
  pImpl->viewSettings_.dof.filter = dofMedian
                                        ? DepthOfFieldOptions::Filter::MEDIAN
                                        : DepthOfFieldOptions::Filter::NONE;
  dirtyFlags_ |= DirtyFlags::DOF;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsDofMedian() {
  return pImpl->viewSettings_.dof.filter == DepthOfFieldOptions::Filter::MEDIAN;
}
#pragma endregion
#pragma region Vignette
void CompositorLayer::SetVignetteEnabled(bool enabled) {
  pImpl->viewSettings_.vignette.enabled = enabled;
  dirtyFlags_ |= DirtyFlags::VIGNETTE;
  // UpdateTimeStamp();
}
bool CompositorLayer::IsVignetteEnabled() {
  return pImpl->viewSettings_.vignette.enabled;
}
void CompositorLayer::SetVignetteMidPoint(float midPoint) {
  pImpl->viewSettings_.vignette.midPoint = midPoint;
  dirtyFlags_ |= DirtyFlags::VIGNETTE;
  // UpdateTimeStamp();
}
float CompositorLayer::GetVignetteMidPoint() {
  return pImpl->viewSettings_.vignette.midPoint;
}
void CompositorLayer::SetVignetteRoundness(float roundness) {
  pImpl->viewSettings_.vignette.roundness = roundness;
  dirtyFlags_ |= DirtyFlags::VIGNETTE;
  // UpdateTimeStamp();
}
float CompositorLayer::GetVignetteRoundness() {
  return pImpl->viewSettings_.vignette.roundness;
}
void CompositorLayer::SetVignetteFeather(float feather) {
  pImpl->viewSettings_.vignette.feather = feather;
  dirtyFlags_ |= DirtyFlags::VIGNETTE;
  // UpdateTimeStamp();
}
float CompositorLayer::GetVignetteFeather() {
  return pImpl->viewSettings_.vignette.feather;
}
void CompositorLayer::SetVignetteColor(const float color[3]) {
  pImpl->viewSettings_.vignette.color.r = color[0];
  pImpl->viewSettings_.vignette.color.g = color[1];
  pImpl->viewSettings_.vignette.color.b = color[2];
}
void CompositorLayer::GetVignetteColor(float color[3]) {
  color[0] = pImpl->viewSettings_.vignette.color.r;
  color[1] = pImpl->viewSettings_.vignette.color.g;
  color[2] = pImpl->viewSettings_.vignette.color.b;
}
#pragma endregion

struct VzCompositor::Impl {
  filament::SwapChain* swapChain_ = nullptr;
  filament::Renderer* renderer_ = nullptr;
  filament::View* compositorView_ = nullptr;
  filament::RenderTarget* compositorRT_[2] = {
      nullptr,
  };
  filament::Texture* compositorRTTexture_[2] = {
      nullptr,
  };
  filament::RenderTarget* tempRT = nullptr;
  filament::Texture* tempRTTexture = nullptr;
  std::list<CompositorLayer*> layers_;
};

VzCompositor::VzCompositor(const VID vid, const std::string& originFrom)
    : VzBaseComp(vid, originFrom, "VzCompositor") , pImpl(new Impl()) {
  assert(gEngine && "native engine is not initialized!");
  pImpl->swapChain_ = gEngine->createSwapChain(width_, height_);
  pImpl->renderer_ = gEngine->createRenderer();
  pImpl->compositorView_ = gEngine->createView();
}

VzCompositor::~VzCompositor() {
  if (gEngine) {
    if (pImpl->swapChain_) gEngine->destroy(pImpl->swapChain_);
    if (pImpl->renderer_) gEngine->destroy(pImpl->renderer_);
    if (pImpl->compositorView_) gEngine->destroy(pImpl->compositorView_);
    for (int i = 0; i < 2; i++) {
      if (pImpl->compositorRT_[i]) gEngine->destroy(pImpl->compositorRT_[i]);
      if (pImpl->compositorRTTexture_[i])
        gEngine->destroy(pImpl->compositorRTTexture_[i]);
    }
    if (pImpl->tempRT) gEngine->destroy(pImpl->tempRT);
    if (pImpl->tempRTTexture) gEngine->destroy(pImpl->tempRTTexture);

    for (auto layerIter = pImpl->layers_.begin();
         layerIter != pImpl->layers_.end(); layerIter++) {
      delete *layerIter;
    }
  }
  delete pImpl;
}

CompositorLayer* VzCompositor::AddLayer(
    const VID vidScene, const VID vidCam,
                            const vzm::VzBaseActor::VISIBLE_LAYER layerMask) {
  CompositorLayer* layer = new CompositorLayer(vidScene, vidCam, layerMask);
  pImpl->layers_.push_back(layer);
  return layer;
}

bool VzCompositor::SetLayerOrder(CompositorLayer* layer, int order) {
  if (order >= pImpl->layers_.size()) {
    return false;
  }

  for (auto iter = pImpl->layers_.begin(); iter != pImpl->layers_.end();) {
    if (*iter == layer) {
      pImpl->layers_.erase(iter);
      break;
    }
    if (++iter == pImpl->layers_.end()) {
      return false;
    }
  }

  auto iter = pImpl->layers_.begin();
  std::advance(iter, order);
  pImpl->layers_.insert(iter, layer);
  return true;
}

uint32_t VzCompositor::GetLayerOrder(CompositorLayer* layer) {
  uint32_t order = 0;
  for (auto iter = pImpl->layers_.begin(); iter != pImpl->layers_.end(); iter++) {
    if (*iter == layer) {
      return order;
    }
    order++;
  }
  return -1;
}

bool VzCompositor::RemoveLayer(int index) {
  if (index >= pImpl->layers_.size()) {
    return false;
  }
  auto iter = pImpl->layers_.begin();
  std::advance(iter, index);
  delete *iter;
  pImpl->layers_.erase(iter);
  return true;
}

bool VzCompositor::RemoveLayer(CompositorLayer* layer) {
  for (auto iter = pImpl->layers_.begin(); iter != pImpl->layers_.end();
       iter++) {
    if (*iter == layer) {
      delete *iter;
      pImpl->layers_.erase(iter);
      return true;
    }
  }
  return false;
}

CompositorLayer* VzCompositor::GetLayer(int index) { 
    if (index >= pImpl->layers_.size()) {
        return nullptr;
    }
    auto iter = pImpl->layers_.begin();
    std::advance(iter, index);
    return *iter; 
}

void VzCompositor::Render() {
  if (!UTILS_HAS_THREADING) {
    gEngine->execute();
  }
  TryResizeRenderTargets();

  ResourceLoader* resource_loader = gEngineApp->GetGltfResourceLoader();
  if (resource_loader) {
    resource_loader->asyncUpdateLoad();

    VzAssetRes* asset_res =
        gEngineApp->GetAssetRes(gEngineApp->activeAsyncAsset);
    if (asset_res && resource_loader->asyncGetLoadProgress() >= 1.) {
      int count = 0;
      gEngineApp->activeAsyncAsset = INVALID_VID;
      filament::gltfio::FFilamentAsset* fasset = downcast(asset_res->asset);
      for (auto& it : asset_res->asyncTextures) {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(it.second);
        tex_res->texture = fasset->mTextures[it.first].texture;
        tex_res->isAsyncLocked = false;
      }
      asset_res->asyncTextures.clear();
    }
  }

  std::unordered_map<AssetVID, std::unique_ptr<VzAssetRes>>& assetResMap =
      *gEngineApp->GetAssetResMap();

  auto& tcm = gEngine->getTransformManager();
  tcm.commitLocalTransformTransaction();

  Renderer::ClearOptions restore_clear_options =
      pImpl->renderer_->getClearOptions();
  Renderer::ClearOptions clear_options;
  clear_options.clearColor = float4{0, 0, 0, 0};
  clear_options.clear = true;
  clear_options.discard = true;
  pImpl->renderer_->setClearOptions(clear_options);

  TimeStamp timer2 = std::chrono::high_resolution_clock::now();

  int currentIdx = 0;
  for (auto iter = pImpl->layers_.begin(); iter != pImpl->layers_.end();
       iter++) {
    CompositorLayer* layer = *iter;
    View* view = layer->pImpl->view_;
    Scene* scene = gEngineApp->GetScene(layer->vidScene_);
    Camera* camera =
        gEngine->getCameraComponent(utils::Entity::import(layer->vidCamera_));
    view->setScene(scene);
    view->setCamera(camera);

    VzCameraRes* cam_res = gEngineApp->GetCameraRes(layer->vidCamera_);

    if (cam_res->FRAMECOUNT == 0) {
      cam_res->timer = std::chrono::high_resolution_clock::now();
    }

    if (cam_res->cameraControllerEnabled) {
      std::chrono::duration<double> time_span =
          std::chrono::duration_cast<std::chrono::duration<double>>(
              timer2 - cam_res->timer);
      cam_res->timer = timer2;
      cam_res->UpdateCameraWithCM((float)time_span.count());
    }

    // Update the cube distortion matrix used for frustum visualization.
    const Camera* lightmapCamera = view->getDirectionalShadowCamera();
    if (lightmapCamera) {
      VzSceneRes* scene_res = gEngineApp->GetSceneRes(layer->vidScene_);
      VzCube* lightmapCube = scene_res->GetLightmapCube();
      lightmapCube->mapFrustum(*gEngine, lightmapCamera);
    }
    VzCube* cameraCube = cam_res->GetCameraCube();
    if (cameraCube) {
      cameraCube->mapFrustum(*gEngine, camera);
    }

    for (auto& it : assetResMap) {
      VzAssetRes* asset_res = it.second.get();
      VzAsset* v_asset = gEngineApp->GetVzComponent<VzAsset>(it.first);
      assert(v_asset);
      vzm::VzAsset::Animator* animator = v_asset->GetAnimator();
      if (animator->IsPlayScene(layer->vidScene_)) {
        animator->UpdateAnimation();
      }
    }

    double3 v = camera->getForwardVector();
    double3 u = camera->getUpVector();
    std::map<Entity, mat4f> restore_billboard_tr;
    scene->forEach([&tcm, &restore_billboard_tr, &u, &v](Entity ett) {
      VID vid = ett.getId();

      VzSceneComp* comp = gEngineApp->GetVzComponent<VzSceneComp>(vid);
      if (comp && comp->IsMatrixAutoUpdate()) {
        comp->UpdateMatrix();
      }

      VzActorRes* actor_res = gEngineApp->GetActorRes(vid);
      if (actor_res && actor_res->isBillboard) {
        auto ti = tcm.getInstance(ett);
        mat4f os2parent = tcm.getTransform(ti);  // local
        restore_billboard_tr[ett] = os2parent;

        mat4 os2ws = (mat4)tcm.getWorldTransform(ti);
        mat4 parent2ws = os2ws * inverse(os2parent);  // fixed
        double4 p_ws_h = os2ws * double4(0, 0, 0, 1);
        double3 p_ws = p_ws_h.xyz / p_ws_h.w;  // fixed

        mat4 os2ws_new = mat4::lookTo(v, p_ws, u);
        mat4 os2parent_new = inverse(parent2ws) * os2ws_new;

        tcm.setTransform(ti, os2parent_new);
      }
    });

    filament::Texture* fogColorTexture =
        gEngineApp->GetSceneRes(layer->vidScene_)->GetIBL()->getFogTexture();
    layer->pImpl->viewSettings_.fogSettings.fogColorTexture = fogColorTexture;
    layer->ApplySettings();

    view->setVisibleLayers(0x3, (uint8_t)layer->layerMask_);
    view->setPostProcessingEnabled(
        (uint8_t)layer->layerMask_ &
        (uint8_t)vzm::VzBaseActor::VISIBLE_LAYER::VISIBLE);
    view->setRenderTarget(pImpl->tempRT);

    pImpl->renderer_->renderStandaloneView(view);

    // compositor
    CompositorQuad* compositor = gEngineApp->GetCompositorQuad();
    pImpl->compositorView_->setCamera(compositor->GetQaudCamera());
    pImpl->compositorView_->setScene(compositor->GetQuadScene());
    pImpl->compositorView_->setPostProcessingEnabled(false);
    pImpl->compositorView_->setRenderTarget(
        pImpl->compositorRT_[currentIdx % 2]);

    MaterialInstance* quad_mi = compositor->GetMaterial();
    if (iter != pImpl->layers_.begin()) {
      quad_mi->setParameter(
          "mainTexture",
          pImpl->compositorRT_[(currentIdx + 1) % 2]->getTexture(
              RenderTarget::AttachmentPoint::COLOR),
          compositor->sampler);
    }
    quad_mi->setParameter(
        "guiTexture",
        pImpl->tempRT->getTexture(RenderTarget::AttachmentPoint::COLOR),
        compositor->sampler);

    //Fence::waitAndDestroy(gEngine->createFence());
    pImpl->renderer_->renderStandaloneView(pImpl->compositorView_);

    for (auto& it : restore_billboard_tr) {
      auto ti = tcm.getInstance(it.first);
      tcm.setTransform(ti, it.second);
    }

    currentIdx++;
  }

  //Fence::waitAndDestroy(gEngine->createFence());
  pImpl->compositorView_->setRenderTarget(nullptr);
  if (pImpl->renderer_->beginFrame(pImpl->swapChain_)) {
    pImpl->renderer_->render(pImpl->compositorView_);
    pImpl->renderer_->endFrame();
  }

  pImpl->renderer_->setClearOptions(restore_clear_options);

  if (gEngine->getBackend() == Backend::OPENGL) {
    Fence::waitAndDestroy(gEngine->createFence());
  }
}

void VzCompositor::Reset() { pImpl->layers_.clear(); }

void VzCompositor::GetCanvas(uint32_t* w, uint32_t* h, float* dpi,
                             void** window) {
  if (w) *w = width_;
  if (h) *h = height_;
  if (dpi) *dpi = dpi_;
  if (window) *window = nativeWindow_;
}

void VzCompositor::SetCanvas(const uint32_t w, const uint32_t h,
                             const float dpi, void* window) {
  // the resize is called during the rendering (pre-processing)
  width_ = w;
  height_ = h;
  dpi_ = dpi;
  nativeWindow_ = window;

  pImpl->compositorView_->setViewport(
      filament::Viewport(0, 0, width_, height_));
}

void VzCompositor::SetFixedTimeUpdate(const float targetFPS) {
  targetFrameRate_ = targetFPS;
  timeStamp_ = std::chrono::high_resolution_clock::now();
}

float VzCompositor::GetFixedTimeUpdate() const { return targetFrameRate_; }

bool VzCompositor::TryResizeRenderTargets() {
  if (gEngine == nullptr) return false;

  colorspaceConversionRequired_ =
      colorSpace_ != SWAP_CHAIN_CONFIG_SRGB_COLORSPACE;

  bool requireUpdateRenderTarget =
      prevWidth_ != width_ || prevHeight_ != height_ || prevDpi_ != dpi_ ||
      prevColorspaceConversionRequired_ != colorspaceConversionRequired_;
  if (!requireUpdateRenderTarget) return false;

  resize();

  prevWidth_ = width_;
  prevHeight_ = height_;
  prevDpi_ = dpi_;
  prevNativeWindow_ = nativeWindow_;
  prevColorspaceConversionRequired_ = colorspaceConversionRequired_;
  return true;
}

void VzCompositor::resize() {
  gEngine->destroy(pImpl->swapChain_);
  if (nativeWindow_ == nullptr) {
    pImpl->swapChain_ = gEngine->createSwapChain(width_, height_);
  } else {
    pImpl->swapChain_ = gEngine->createSwapChain(
        nativeWindow_, filament::SwapChain::CONFIG_HAS_STENCIL_BUFFER);
  }

  gEngine->destroy(pImpl->tempRTTexture);
  gEngine->destroy(pImpl->tempRT);
  pImpl->tempRTTexture =
      Texture::Builder()
          .width(width_)
          .height(height_)
          .levels(1)
          .usage(TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE)
          .format(TextureFormat::RGBA8)
          .build(*gEngine);
  pImpl->tempRT =
      RenderTarget::Builder()
          .texture(RenderTarget::AttachmentPoint::COLOR, pImpl->tempRTTexture)
          //.texture(RenderTarget::AttachmentPoint::DEPTH, rtDepthTexture_)
          .build(*gEngine);

  for (int i = 0; i < 2; i++) {
    gEngine->destroy(pImpl->compositorRT_[i]);
    gEngine->destroy(pImpl->compositorRTTexture_[i]);

    pImpl->compositorRTTexture_[i] =
        Texture::Builder()
            .width(width_)
            .height(height_)
            .levels(1)
            .usage(TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE)
            .format(TextureFormat::RGBA8)
            .build(*gEngine);
    pImpl->compositorRT_[i] =
        RenderTarget::Builder()
            .texture(RenderTarget::AttachmentPoint::COLOR,
                     pImpl->compositorRTTexture_[i])
            //.texture(RenderTarget::AttachmentPoint::DEPTH, rtDepthTexture_)
            .build(*gEngine);
  }
}

}  // namespace vzm
