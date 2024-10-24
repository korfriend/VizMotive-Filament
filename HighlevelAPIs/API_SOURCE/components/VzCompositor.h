#pragma once
#include "../VizComponentAPIs.h"
#include "VzActor.h"

namespace vzm {

struct API_EXPORT CompositorLayer {
 private:
  VID vidScene_;
  VID vidCamera_;
  vzm::VzBaseActor::VISIBLE_LAYER layerMask_ =
      vzm::VzBaseActor::VISIBLE_LAYER::VISIBLE;
  enum class DirtyFlags : uint32_t {
    NONE = 0,
    ANTI_ALIASING = 1 << 0,
    TAA = 1 << 1,
    MSAA = 1 << 2,
    DSR = 1 << 3,
    SSAO = 1 << 4,
    SCREEN_SPACE_REFLECTIONS = 1 << 5,
    BLOOM = 1 << 6,
    FOG = 1 << 7,
    DOF = 1 << 8,
    VIGNETTE = 1 << 9,
    DITHERING = 1 << 10,
    RENDER_QUALITY = 1 << 11,
    DYNAMIC_LIGHTING = 1 << 12,
    SHADOW_TYPE = 1 << 13,
    VSM_SHADOW_OPTIONS = 1 << 14,
    SOFT_SHADOW_OPTIONS = 1 << 15,
    GUARD_BAND = 1 << 16,
    STEREOSCOPIC_OPTIONS = 1 << 17,
    POST_PROCESSING_ENABLED = 1 << 18,
    ALL = 0xFFFFFFFF
  };
  DirtyFlags dirtyFlags_ = DirtyFlags::ALL;

  struct Impl;
  Impl* pImpl;

  void ApplySettings();
 public:
  CompositorLayer(VID vidScene, VID vidCamera,
                  vzm::VzBaseActor::VISIBLE_LAYER layerMask);
  ~CompositorLayer();

  void SetVisibleLayerMask(const uint32_t layerBits, const uint8_t maskBits);

  void SetViewport(const uint32_t x, const uint32_t y, const uint32_t w,
                   const uint32_t h);
  void GetViewport(uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h);

  void SetScene(VID vidScene);
  VID GetScene() const;

  void SetCamera(VID vidCamera);
  VID GetCamera() const;

  void ApplyViewSettingsFrom(CompositorLayer* other);

  void SetPostProcessingEnabled(bool enabled);
  bool IsPostProcessingEnabled();

  void SetDitheringEnabled(bool enabled);
  bool IsDitheringEnabled();

  void SetBloomEnabled(bool enabled);
  bool IsBloomEnabled();

  void SetTaaEnabled(bool enabled);
  bool IsTaaEnabled();

  void SetFxaaEnabled(bool enabled);
  bool IsFxaaEnabled();

  void SetMsaaEnabled(bool enabled);
  bool IsMsaaEnabled();

  void SetMsaaSampleCount(int samples);
  int GetMsaaSampleCount();

  void SetMsaaCustomResolve(bool customResolve);
  bool IsMsaaCustomResolve();

  void SetSsaoEnabled(bool enabled);
  bool IsSsaoEnabled();

  void SetScreenSpaceReflectionsEnabled(bool enabled);
  bool IsScreenSpaceReflectionsEnabled();

  void SetGuardBandEnabled(bool enabled);
  bool IsGuardBandEnabled();

  void SetBloomStrength(float strength);
  float GetBloomStrength();

  void SetBloomThreshold(bool threshold);
  bool IsBloomThreshold();

  void SetBloomLevels(int levels);
  int GetBloomLevels();

  void SetBloomQuality(int quality);
  int GetBloomQuality();

  void SetBloomLensFlare(bool lensFlare);
  bool IsBloomLensFlare();

  void SetTaaUpscaling(bool upscaling);
  bool IsTaaUpscaling();

  void SetTaaHistoryReprojection(bool historyReprojection);
  bool IsTaaHistoryReprojection();

  void SetTaaFeedback(float feedback);
  float GetTaaFeedback();

  void SetTaaFilterHistory(bool filterHistory);
  bool IsTaaFilterHistory();

  void SetTaaFilterInput(bool filterInput);
  bool IsTaaFilterInput();

  void SetTaaFilterWidth(float filterWidth);
  float GetTaaFilterWidth();

  void SetTaaLodBias(float lodBias);
  float GetTaaLodBias();

  void SetTaaUseYCoCg(bool useYCoCg);
  bool IsTaaUseYCoCg();

  void SetTaaPreventFlickering(bool preventFlickering);
  bool IsTaaPreventFlickering();

  enum class JitterPattern : uint8_t {
    RGSS_X4,           //!  4-samples, rotated grid sampling
    UNIFORM_HELIX_X4,  //!  4-samples, uniform grid in helix sequence
    HALTON_23_X8,      //!  8-samples of halton 2,3
    HALTON_23_X16,     //! 16-samples of halton 2,3
    HALTON_23_X32      //! 32-samples of halton 2,3
  };
  void SetTaaJitterPattern(JitterPattern jitterPattern);
  JitterPattern GetTaaJitterPattern();

  enum class BoxClipping : uint8_t {
    ACCURATE,  //!< Accurate box clipping
    CLAMP,     //!< clamping
    NONE       //!< no rejections (use for debugging)
  };
  void SetTaaBoxClipping(BoxClipping boxClipping);
  BoxClipping GetTaaBoxClipping();

  enum class BoxType : uint8_t {
    AABB,          //!< use an AABB neighborhood
    VARIANCE,      //!< use the variance of the neighborhood (not recommended)
    AABB_VARIANCE  //!< use both AABB and variance
  };
  void SetTaaBoxType(BoxType boxType);
  BoxType GetTaaBoxType();

  void SetTaaVarianceGamma(float varianceGamma);
  float GetTaaVarianceGamma();

  void SetTaaSharpness(float sharpness);
  float GetTaaSharpness();

  void SetSsaoQuality(int quality);
  int GetSsaoQuality();

  void SetSsaoLowPassFilter(int lowPassFilter);
  int GetSsaoLowPassFilter();

  void SetSsaoBentNormals(bool bentNormals);
  bool IsSsaoBentNormals();

  void SetSsaoUpsampling(bool upsampling);
  bool IsSsaoUpsampling();

  void SetSsaoMinHorizonAngleRad(float minHorizonAngleRad);
  float GetSsaoMinHorizonAngleRad();

  void SetSsaoBilateralThreshold(float bilateralThreshold);
  float GetSsaoBilateralThreshold();

  void SetSsaoHalfResolution(bool halfResolution);
  bool IsSsaoHalfResolution();

  void SetSsaoSsctEnabled(bool enabled);
  bool IsSsaoSsctEnabled();

  void SetSsaoSsctLightConeRad(float lightConeRad);
  float GetSsaoSsctLightConeRad();

  void SetSsaoSsctShadowDistance(float shadowDistance);
  float GetSsaoSsctShadowDistance();

  void SetSsaoSsctContactDistanceMax(float contactDistanceMax);
  float GetSsaoSsctContactDistanceMax();

  void SetSsaoSsctIntensity(float intensity);
  float GetSsaoSsctIntensity();

  void SetSsaoSsctDepthBias(float depthBias);
  float GetSsaoSsctDepthBias();

  void SetSsaoSsctDepthSlopeBias(float depthSlopeBias);
  float GetSsaoSsctDepthSlopeBias();

  void SetSsaoSsctSampleCount(int sampleCount);
  int GetSsaoSsctSampleCount();

  void SetSsaoSsctLightDirection(const float lightDirection[3]);
  void GetSsaoSsctLightDirection(float lightDirection[3]);

  void SetScreenSpaceReflectionsThickness(float thickness);
  float GetScreenSpaceReflectionsThickness();

  void SetScreenSpaceReflectionsBias(float bias);
  float GetScreenSpaceReflectionsBias();

  void SetScreenSpaceReflectionsMaxDistance(float maxDistance);
  float GetScreenSpaceReflectionsMaxDistance();

  void SetScreenSpaceReflectionsStride(float stride);
  float GetScreenSpaceReflectionsStride();

  void SetDynamicResoultionEnabled(bool enabled);
  bool IsDynamicResoultionEnabled();

  void SetDynamicResoultionHomogeneousScaling(bool homogeneousScaling);
  bool IsDynamicResoultionHomogeneousScaling();

  void SetDynamicResoultionMinScale(float minScale);
  float GetDynamicResoultionMinScale();

  void SetDynamicResoultionMaxScale(float maxScale);
  float GetDynamicResoultionMaxScale();

  void SetDynamicResoultionQuality(int quality);
  int GetDynamicResoultionQuality();

  void SetDynamicResoultionSharpness(float sharpness);
  float GetDynamicResoultionSharpness();

  enum class ShadowType : uint8_t {
    PCF,   //!< percentage-closer filtered shadows (default)
    VSM,   //!< variance shadows
    DPCF,  //!< PCF with contact hardening simulation
    PCSS,  //!< PCF with soft shadows and contact hardening
    PCFd,  // for debugging only, don't use.
  };
  void SetShadowType(ShadowType shadowType);
  ShadowType GetShadowType();

  void SetVsmHighPrecision(bool highPrecision);
  bool IsVsmHighPrecision();

  void SetVsmMsaaSamples(int msaaSamples);
  int GetVsmMsaaSamples();

  void SetVsmAnisotropy(int anisotropy);
  int GetVsmAnisotropy();

  void SetVsmMipmapping(bool mipmapping);
  bool IsVsmMipmapping();

  void SetSoftShadowPenumbraScale(float penumbraScale);
  float GetSoftShadowPenumbraScale();

  void SetSoftShadowPenumbraRatioScale(float penumbraRatioScale);
  float GetSoftShadowPenumbraRatioScale();

  void SetFogEnabled(bool enabled);
  bool IsFogEnabled();

  void SetFogDistance(float distance);
  float GetFogDistance();

  void SetFogDensity(float density);
  float GetFogDensity();

  void SetFogHeight(float height);
  float GetFogHeight();

  void SetFogHeightFalloff(float heightFalloff);
  float GetFogHeightFalloff();

  void SetFogInScatteringStart(float inScatteringStart);
  float GetFogInScatteringStart();

  void SetFogInScatteringSize(float inScatteringSize);
  float GetFogInScatteringSize();

  void SetFogExcludeSkybox(bool excludeSkybox);
  bool IsFogExcludeSkybox();

  enum class FogColorSource : uint8_t { CONSTANT, IBL, SKYBOX };
  void SetFogColorSource(FogColorSource fogColorSource);
  FogColorSource GetFogColorSource();

  void SetFogColor(const float color[3]);
  void GetFogColor(float color[3]);

  void SetDofEnabled(bool enabled);
  bool IsDofEnabled();

  void SetDofCocScale(float cocScale);
  float GetDofCocScale();

  void SetDofCocAspectRatio(float cocAspectRatio);
  float GetDofCocAspectRatio();

  void SetDofRingCount(int dofRingCount);
  int GetDofRingCount();

  void SetDofMaxCoc(int maxCoc);
  int GetDofMaxCoc();

  void SetDofNativeResolution(bool nativeResolution);
  bool IsDofNativeResolution();

  void SetDofMedian(bool dofMedian);
  bool IsDofMedian();

  void SetVignetteEnabled(bool enabled);
  bool IsVignetteEnabled();

  void SetVignetteMidPoint(float midPoint);
  float GetVignetteMidPoint();

  void SetVignetteRoundness(float roundness);
  float GetVignetteRoundness();

  void SetVignetteFeather(float feather);
  float GetVignetteFeather();

  void SetVignetteColor(const float color[3]);
  void GetVignetteColor(float color[3]);

  friend struct CompositorLayer;
  friend struct VzCompositor;
};

struct API_EXPORT VzCompositor : VzBaseComp {
 public:
  VzCompositor(const VID vid, const std::string& originFrom);
  ~VzCompositor();

  CompositorLayer* AddLayer(const VID vidScene, const VID vidCam,
                            const vzm::VzBaseActor::VISIBLE_LAYER layerMask);
  bool SetLayerOrder(CompositorLayer* layer, int order);
  uint32_t GetLayerOrder(CompositorLayer* layer);
  bool RemoveLayer(int index);
  bool RemoveLayer(CompositorLayer* layer);

  CompositorLayer* GetLayer(int index);

  void Render();

  void Reset();

  void GetCanvas(uint32_t* w, uint32_t* h, float* dpi, void** window = nullptr);
  void SetCanvas(const uint32_t w, const uint32_t h, const float dpi,
                 void* window);

  void SetFixedTimeUpdate(const float targetFPS);
  float GetFixedTimeUpdate() const;

 private:
  bool TryResizeRenderTargets();
  void resize();

  // canvas
  uint32_t width_ = 16u;
  uint32_t height_ = 16u;
  float dpi_ = 96.f;
  float scaling_ = 1;  // custom DPI scaling factor (optional)
  void* nativeWindow_ = nullptr;
  uint32_t prevWidth_ = 0;
  uint32_t prevHeight_ = 0;
  float prevDpi_ = 0;
  void* prevNativeWindow_ = nullptr;
  bool prevColorspaceConversionRequired_ = false;

  TimeStamp timeStamp_ = {};
  float targetFrameRate_ = 60.f;
  bool colorspaceConversionRequired_ = false;
  uint64_t colorSpace_ = 0;  // swapchain color space

  struct Impl;
  Impl* pImpl;
};
}  // namespace vzm
