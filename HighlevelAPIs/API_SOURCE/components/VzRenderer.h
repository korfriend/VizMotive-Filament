///
/// @file      VzRenderer.h
/// @brief     The header file for the VzRenderer class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZRENDERER_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZRENDERER_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzRenderer
/// @brief
/// The class for the renderer.
struct API_EXPORT VzRenderer : VzBaseComp {
  using PickCallback = void (*)(VID);

  /// @VzRenderer
  /// @brief
  /// Constructor for the VzRenderer class.
  /// @param[in] vid
  /// The unique identifier for the renderer.
  /// @param[in] originFrom
  /// The name of the function that created the renderer.
  VzRenderer(const VID vid, const std::string& originFrom)
      : VzBaseComp(vid, originFrom, "VzRenderer") {}

  /// @fn SetCanvas
  /// @brief
  /// Set the canvas.
  /// @param[in] w
  /// The width of the canvas.
  /// @param[in] h
  /// The height of the canvas.
  /// @param[in] dpi
  /// The DPI of the canvas.
  /// @param[in] window
  /// The window.
  void SetCanvas(const uint32_t w, const uint32_t h, const float dpi,
                 void* window = nullptr);

  /// @fn GetCanvas
  /// @brief
  /// Get the canvas.
  /// @param[out] w
  /// The width of the canvas.
  /// @param[out] h
  /// The height of the canvas.
  /// @param[out] dpi
  /// The DPI of the canvas.
  /// @param[out] window
  /// The window.
  void GetCanvas(uint32_t* w, uint32_t* h, float* dpi, void** window = nullptr);

  /// @fn SetViewport
  /// @brief
  /// Set the viewport.
  /// @param[in] x
  /// The x-coordinate of the viewport.
  /// @param[in] y
  /// The y-coordinate of the viewport.
  /// @param[in] w
  /// The width of the viewport.
  /// @param[in] h
  /// The height of the viewport.
  void SetViewport(const uint32_t x, const uint32_t y, const uint32_t w,
                   const uint32_t h);

  /// @fn GetViewport
  /// @brief
  /// Get the viewport.
  /// @param[out] x
  /// The x-coordinate of the viewport.
  /// @param[out] y
  /// The y-coordinate of the viewport.
  /// @param[out] w
  /// The width of the viewport.
  void GetViewport(uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h);

  /// @fn SetVisibleLayerMask
  /// @brief
  /// Set the visible layer mask.
  /// @param[in] layerBits
  /// The layer bits.
  /// @param[in] maskBits
  /// The mask bits.
  void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);

  /// @fn Pick
  /// @brief
  /// Pick the object.
  /// @param[in] x
  /// The x-coordinate.
  /// @param[in] y
  /// The y-coordinate.
  /// @param[in] callback
  /// The callback.
  void Pick(const uint32_t x, const uint32_t y, PickCallback callback);

  /// @fn IntersectActors
  /// @brief
  /// Intersect the actors.
  /// @param[in] x
  /// The x-coordinate.
  /// @param[in] y
  /// The y-coordinate.
  /// @param[in] vidCam
  /// The unique identifier for the camera.
  /// @param[in] vidActors
  /// The unique identifiers for the actors.
  /// @param[out] hitResults
  /// The hit results.
  /// @param[in] recursive
  /// The recursive flag.
  /// @return
  /// The number of intersected actors.
  size_t IntersectActors(const uint32_t x, const uint32_t y, const VID vidCam,
                         const std::vector<VID>& vidActors,
                         std::vector<HitResult>& hitResults,
                         const bool recursive = true);

  /// @fn SetPostProcessingEnabled
  /// @brief
  /// Set post-processing enabled.
  /// @param[in] enabled
  /// The flag to enable post-processing.
  void SetPostProcessingEnabled(bool enabled);

  /// @fn IsPostProcessingEnabled
  /// @brief
  /// Check if post-processing is enabled.
  /// @return
  /// true if post-processing is enabled, false otherwise.
  bool IsPostProcessingEnabled();

  /// @fn SetDitheringEnabled
  /// @brief
  /// Set dithering enabled.
  /// @param[in] enabled
  /// The flag to enable dithering.
  void SetDitheringEnabled(bool enabled);

  /// @fn IsDitheringEnabled
  /// @brief
  /// Check if dithering is enabled.
  /// @return
  /// true if dithering is enabled, false otherwise.
  bool IsDitheringEnabled();

  /// @fn SetBloomEnabled
  /// @brief
  /// Set bloom enabled.
  /// @param[in] enabled
  /// The flag to enable bloom.
  void SetBloomEnabled(bool enabled);

  /// @fn IsBloomEnabled
  /// @brief
  /// Check if bloom is enabled.
  /// @return
  /// true if bloom is enabled, false otherwise.
  bool IsBloomEnabled();

  /// @fn SetTaaEnabled
  /// @brief
  /// Set TAA enabled.
  /// @param[in] enabled
  /// The flag to enable TAA.
  void SetTaaEnabled(bool enabled);

  /// @fn IsTaaEnabled
  /// @brief
  /// Check if TAA is enabled.
  /// @return
  /// true if TAA is enabled, false otherwise.
  bool IsTaaEnabled();

  /// @fn SetFxaaEnabled
  /// @brief
  /// Set FXAA enabled.
  /// @param[in] enabled
  /// The flag to enable FXAA.
  void SetFxaaEnabled(bool enabled);

  /// @fn IsFxaaEnabled
  /// @brief
  /// Check if FXAA is enabled.
  /// @return
  /// true if FXAA is enabled, false otherwise.
  bool IsFxaaEnabled();

  /// @fn SetMsaaEnabled
  /// @brief
  /// Set MSAA enabled.
  /// @param[in] enabled
  /// The flag to enable MSAA.
  void SetMsaaEnabled(bool enabled);

  /// @fn IsMsaaEnabled
  /// @brief
  /// Check if MSAA is enabled.
  /// @return
  /// true if MSAA is enabled, false otherwise.
  bool IsMsaaEnabled();

  /// @fn SetMsaaSampleCount
  /// @brief
  /// Set the MSAA sample count.
  /// @param[in] samples
  /// The MSAA sample count.
  void SetMsaaSampleCount(int samples);

  /// @fn GetMsaaSampleCount
  /// @brief
  /// Get the MSAA sample count.
  /// @return
  /// The MSAA sample count.
  int GetMsaaSampleCount();

  /// @fn SetMsaaCustomResolve
  /// @brief
  /// Set MSAA custom resolve.
  /// @param[in] customResolve
  /// The flag to enable MSAA custom resolve.
  void SetMsaaCustomResolve(bool customResolve);

  /// @fn IsMsaaCustomResolve
  /// @brief
  /// Check if MSAA custom resolve is enabled.
  /// @return
  /// true if MSAA custom resolve is enabled, false otherwise.
  bool IsMsaaCustomResolve();

  /// @fn SetSsaoEnabled
  /// @brief
  /// Set SSAO enabled.
  /// @param[in] enabled
  /// The flag to enable SSAO.
  void SetSsaoEnabled(bool enabled);

  /// @fn IsSsaoEnabled
  /// @brief
  /// Check if SSAO is enabled.
  /// @return
  /// true if SSAO is enabled, false otherwise.
  bool IsSsaoEnabled();

  /// @fn SetScreenSpaceReflectionsEnabled
  /// @brief
  /// Set screen space reflections enabled.
  /// @param[in] enabled
  /// The flag to enable screen space reflections.
  void SetScreenSpaceReflectionsEnabled(bool enabled);

  /// @fn IsScreenSpaceReflectionsEnabled
  /// @brief
  /// Check if screen space reflections are enabled.
  /// @return
  /// true if screen space reflections are enabled, false otherwise.
  bool IsScreenSpaceReflectionsEnabled();

  /// @fn SetGuardBandEnabled
  /// @brief
  /// Set guard band enabled.
  /// @param[in] enabled
  /// The flag to enable guard band.
  void SetGuardBandEnabled(bool enabled);

  /// @fn IsGuardBandEnabled
  /// @brief
  /// Check if guard band is enabled.
  /// @return
  /// true if guard band is enabled, false otherwise.
  bool IsGuardBandEnabled();

  /// @fn SetBloomStrength
  /// @brief
  /// Set the bloom strength.
  /// @param[in] strength
  /// The bloom strength.
  void SetBloomStrength(float strength);

  /// @fn GetBloomStrength
  /// @brief
  /// Get the bloom strength.
  /// @return
  /// The bloom strength.
  float GetBloomStrength();

  /// @fn SetBloomThreshold
  /// @brief
  /// Set the bloom threshold.
  /// @param[in] threshold
  /// The flag to enable the bloom threshold.
  void SetBloomThreshold(bool threshold);

  /// @fn IsBloomThreshold
  /// @brief
  /// Check if the bloom threshold is enabled.
  /// @return
  /// true if the bloom threshold is enabled, false otherwise.
  bool IsBloomThreshold();

  /// @fn SetBloomLevels
  /// @brief
  /// Set the bloom levels.
  /// @param[in] levels
  /// The bloom levels.
  void SetBloomLevels(int levels);

  /// @fn GetBloomLevels
  /// @brief
  /// Get the bloom levels.
  /// @return
  /// The bloom levels.
  int GetBloomLevels();

  /// @fn SetBloomQuality
  /// @brief
  /// Set the bloom quality.
  /// @param[in] quality
  /// The bloom quality.
  void SetBloomQuality(int quality);

  /// @fn GetBloomQuality
  /// @brief
  /// Get the bloom quality.
  /// @return
  /// The bloom quality.
  int GetBloomQuality();

  /// @fn SetBloomLensFlare
  /// @brief
  /// Set bloom lens flare.
  /// @param[in] lensFlare
  /// The flag to enable bloom lens flare.
  void SetBloomLensFlare(bool lensFlare);

  /// @fn IsBloomLensFlare
  /// @brief
  /// Check if bloom lens flare is enabled.
  /// @return
  /// true if bloom lens flare is enabled, false otherwise.
  bool IsBloomLensFlare();

  /// @fn SetBloomLensFlareStrength
  /// @brief
  /// Set the bloom lens flare strength.
  /// @param[in] strength
  /// The bloom lens flare strength.
  void SetTaaUpscaling(bool upscaling);

  /// @fn IsTaaUpscaling
  /// @brief
  /// Check if TAA upscaling is enabled.
  /// @return
  /// true if TAA upscaling is enabled, false otherwise.
  bool IsTaaUpscaling();

  /// @fn SetTaaHistoryReprojection
  /// @brief
  /// Set TAA history reprojection.
  /// @param[in] historyReprojection
  /// The flag to enable TAA history reprojection.
  void SetTaaHistoryReprojection(bool historyReprojection);

  /// @fn IsTaaHistoryReprojection
  /// @brief
  /// Check if TAA history reprojection is enabled.
  /// @return
  /// true if TAA history reprojection is enabled, false otherwise.
  bool IsTaaHistoryReprojection();

  /// @fn SetTaaFeedback
  /// @brief
  /// Set TAA feedback.
  /// @param[in] feedback
  /// The TAA feedback.
  void SetTaaFeedback(float feedback);

  /// @fn GetTaaFeedback
  /// @brief
  /// Get the TAA feedback.
  /// @return
  /// The TAA feedback.
  float GetTaaFeedback();

  /// @fn SetTaaFilterHistory
  /// @brief
  /// Set TAA filter history.
  /// @param[in] filterHistory
  /// The flag to enable TAA filter history.
  void SetTaaFilterHistory(bool filterHistory);

  /// @fn IsTaaFilterHistory
  /// @brief
  /// Check if TAA filter history is enabled.
  /// @return
  /// true if TAA filter history is enabled, false otherwise.
  bool IsTaaFilterHistory();

  /// @fn SetTaaFilterInput
  /// @brief
  /// Set TAA filter input.
  /// @param[in] filterInput
  /// The flag to enable TAA filter input.
  void SetTaaFilterInput(bool filterInput);

  /// @fn IsTaaFilterInput
  /// @brief
  /// Check if TAA filter input is enabled.
  /// @return
  /// true if TAA filter input is enabled, false otherwise.
  bool IsTaaFilterInput();

  /// @fn SetTaaFilterWidth
  /// @brief
  /// Set TAA filter width.
  /// @param[in] filterWidth
  /// The TAA filter width.
  void SetTaaFilterWidth(float filterWidth);

  /// @fn GetTaaFilterWidth
  /// @brief
  /// Get the TAA filter width.
  /// @return
  /// The TAA filter width.
  float GetTaaFilterWidth();

  /// @fn SetTaaLodBias
  /// @brief
  /// Set TAA LOD bias.
  /// @param[in] lodBias
  /// The TAA LOD bias.
  void SetTaaLodBias(float lodBias);

  /// @fn GetTaaLodBias
  /// @brief
  /// Get the TAA LOD bias.
  /// @return
  /// The TAA LOD bias.
  float GetTaaLodBias();

  /// @fn SetTaaUseYCoCg
  /// @brief
  /// Set TAA use YCoCg.
  /// @param[in] useYCoCg
  /// The flag to enable TAA use YCoCg.
  void SetTaaUseYCoCg(bool useYCoCg);

  /// @fn IsTaaUseYCoCg
  /// @brief
  /// Check if TAA use YCoCg is enabled.
  /// @return
  /// true if TAA use YCoCg is enabled, false otherwise.
  bool IsTaaUseYCoCg();

  /// @fn SetTaaPreventFlickering
  /// @brief
  /// Set TAA prevent flickering.
  /// @param[in] preventFlickering
  /// The flag to enable TAA prevent flickering.
  void SetTaaPreventFlickering(bool preventFlickering);

  /// @fn IsTaaPreventFlickering
  /// @brief
  /// Check if TAA prevent flickering is enabled.
  /// @return
  /// true if TAA prevent flickering is enabled, false otherwise.
  bool IsTaaPreventFlickering();

  /// @enum JitterPattern
  /// @brief
  /// The jitter pattern.
  enum class JitterPattern : uint8_t {
    RGSS_X4,           //!  4-samples, rotated grid sampling
    UNIFORM_HELIX_X4,  //!  4-samples, uniform grid in helix sequence
    HALTON_23_X8,      //!  8-samples of halton 2,3
    HALTON_23_X16,     //! 16-samples of halton 2,3
    HALTON_23_X32      //! 32-samples of halton 2,3
  };

  /// @fn SetTaaJitterPattern
  /// @brief
  /// Set TAA jitter pattern.
  /// @param[in] jitterPattern
  /// The TAA jitter pattern.
  void SetTaaJitterPattern(JitterPattern jitterPattern);

  /// @fn GetTaaJitterPattern
  /// @brief
  /// Get the TAA jitter pattern.
  /// @return
  /// The TAA jitter pattern.
  JitterPattern GetTaaJitterPattern();

  /// @enum BoxClipping
  /// @brief
  /// The box clipping.
  enum class BoxClipping : uint8_t {
    ACCURATE,  //!< Accurate box clipping
    CLAMP,     //!< clamping
    NONE       //!< no rejections (use for debugging)
  };

  /// @fn SetTaaBoxClipping
  /// @brief
  /// Set TAA box clipping.
  /// @param[in] boxClipping
  /// The TAA box clipping.
  void SetTaaBoxClipping(BoxClipping boxClipping);

  /// @fn GetTaaBoxClipping
  /// @brief
  /// Get the TAA box clipping.
  /// @return
  /// The TAA box clipping.
  BoxClipping GetTaaBoxClipping();

  /// @enum BoxType
  /// @brief
  /// The box type.
  enum class BoxType : uint8_t {
    AABB,          //!< use an AABB neighborhood
    VARIANCE,      //!< use the variance of the neighborhood (not recommended)
    AABB_VARIANCE  //!< use both AABB and variance
  };

  /// @fn SetTaaBoxType
  /// @brief
  /// Set TAA box type.
  /// @param[in] boxType
  /// The TAA box type.
  void SetTaaBoxType(BoxType boxType);

  /// @fn GetTaaBoxType
  /// @brief
  /// Get the TAA box type.
  /// @return
  /// The TAA box type.
  BoxType GetTaaBoxType();

  /// @fn SetTaaVarianceGamma
  /// @brief
  /// Set TAA variance gamma.
  /// @param[in] varianceGamma
  /// The TAA variance gamma.
  void SetTaaVarianceGamma(float varianceGamma);

  /// @fn GetTaaVarianceGamma
  /// @brief
  /// Get the TAA variance gamma.
  /// @return
  /// The TAA variance gamma.
  float GetTaaVarianceGamma();

  /// @fn SetTaaSharpness
  /// @brief
  /// Set TAA sharpness.
  /// @param[in] sharpness
  /// The TAA sharpness.
  void SetTaaSharpness(float sharpness);

  /// @fn GetTaaSharpness
  /// @brief
  /// Get the TAA sharpness.
  /// @return
  /// The TAA sharpness.
  float GetTaaSharpness();

  /// @fn SetSsaoQuality
  /// @brief
  /// Set SSAO quality.
  /// @param[in] quality
  /// The SSAO quality.
  void SetSsaoQuality(int quality);

  /// @fn GetSsaoQuality
  /// @brief
  /// Get the SSAO quality.
  /// @return
  /// The SSAO quality.
  int GetSsaoQuality();

  /// @fn SetSsaoRadius
  /// @brief
  /// Set SSAO radius.
  /// @param[in] radius
  /// The SSAO radius.
  void SetSsaoLowPassFilter(int lowPassFilter);

  /// @fn GetSsaoLowPassFilter
  /// @brief
  /// Get the SSAO low pass filter.
  /// @return
  /// The SSAO low pass filter.
  int GetSsaoLowPassFilter();

  /// @fn SetSsaoBentNormals
  /// @brief
  /// Set SSAO bent normals.
  /// @param[in] bentNormals
  /// The flag to enable SSAO bent normals.
  void SetSsaoBentNormals(bool bentNormals);

  /// @fn IsSsaoBentNormals
  /// @brief
  /// Check if SSAO bent normals are enabled.
  /// @return
  /// true if SSAO bent normals are enabled, false otherwise.
  bool IsSsaoBentNormals();

  /// @fn SetSsaoUpsampling
  /// @brief
  /// Set SSAO upsampling.
  /// @param[in] upsampling
  /// The flag to enable SSAO upsampling.
  void SetSsaoUpsampling(bool upsampling);

  /// @fn IsSsaoUpsampling
  /// @brief
  /// Check if SSAO upsampling is enabled.
  /// @return
  /// true if SSAO upsampling is enabled, false otherwise.
  bool IsSsaoUpsampling();

  /// @fn SetSsaoMinHorizonAngleRad
  /// @brief
  /// Set SSAO min horizon angle.
  /// @param[in] minHorizonAngleRad
  /// The SSAO min horizon angle.
  void SetSsaoMinHorizonAngleRad(float minHorizonAngleRad);

  /// @fn GetSsaoMinHorizonAngleRad
  /// @brief
  /// Get the SSAO min horizon angle.
  /// @return
  /// The SSAO min horizon angle.
  float GetSsaoMinHorizonAngleRad();

  /// @fn SetSsaoBilateralThreshold
  /// @brief
  /// Set SSAO bilateral threshold.
  /// @param[in] bilateralThreshold
  /// The SSAO bilateral threshold.
  void SetSsaoBilateralThreshold(float bilateralThreshold);

  /// @fn GetSsaoBilateralThreshold
  /// @brief
  /// Get the SSAO bilateral threshold.
  /// @return
  /// The SSAO bilateral threshold.
  float GetSsaoBilateralThreshold();

  /// @fn SetSsaoHalfResolution
  /// @brief
  /// Set SSAO half resolution.
  /// @param[in] halfResolution
  /// The flag to enable SSAO half resolution.
  void SetSsaoHalfResolution(bool halfResolution);

  /// @fn IsSsaoHalfResolution
  /// @brief
  /// Check if SSAO half resolution is enabled.
  /// @return
  /// true if SSAO half resolution is enabled, false otherwise.
  bool IsSsaoHalfResolution();

  /// @fn SetSsaoSsctEnabled
  /// @brief
  /// Set SSAO SSCT enabled.
  /// @param[in] enabled
  /// The flag to enable SSAO SSCT.
  void SetSsaoSsctEnabled(bool enabled);

  /// @fn IsSsaoSsctEnabled
  /// @brief
  /// Check if SSAO SSCT is enabled.
  /// @return
  /// true if SSAO SSCT is enabled, false otherwise.
  bool IsSsaoSsctEnabled();

  /// @fn SetSsaoSsctLightConeRad
  /// @brief
  /// Set SSAO SSCT light cone radius.
  /// @param[in] lightConeRad
  /// The SSAO SSCT light cone radius.
  void SetSsaoSsctLightConeRad(float lightConeRad);

  /// @fn GetSsaoSsctLightConeRad
  /// @brief
  /// Get the SSAO SSCT light cone radius.
  /// @return
  /// The SSAO SSCT light cone radius.
  float GetSsaoSsctLightConeRad();

  /// @fn SetSsaoSsctShadowDistance
  /// @brief
  /// Set SSAO SSCT shadow distance.
  /// @param[in] shadowDistance
  /// The SSAO SSCT shadow distance.
  void SetSsaoSsctShadowDistance(float shadowDistance);

  /// @fn GetSsaoSsctShadowDistance
  /// @brief
  /// Get the SSAO SSCT shadow distance.
  /// @return
  /// The SSAO SSCT shadow distance.
  float GetSsaoSsctShadowDistance();

  /// @fn SetSsaoSsctContactDistanceMax
  /// @brief
  /// Set SSAO SSCT contact distance max.
  /// @param[in] contactDistanceMax
  /// The SSAO SSCT contact distance max.
  void SetSsaoSsctContactDistanceMax(float contactDistanceMax);

  /// @fn GetSsaoSsctContactDistanceMax
  /// @brief
  /// Get the SSAO SSCT contact distance max.
  /// @return
  /// The SSAO SSCT contact distance max.
  float GetSsaoSsctContactDistanceMax();

  /// @fn SetSsaoSsctIntensity
  /// @brief
  /// Set SSAO SSCT intensity.
  /// @param[in] intensity
  /// The SSAO SSCT intensity.
  void SetSsaoSsctIntensity(float intensity);

  /// @fn GetSsaoSsctIntensity
  /// @brief
  /// Get the SSAO SSCT intensity.
  /// @return
  /// The SSAO SSCT intensity.
  float GetSsaoSsctIntensity();

  /// @fn SetSsaoSsctDepthBias
  /// @brief
  /// Set SSAO SSCT depth bias.
  /// @param[in] depthBias
  /// The SSAO SSCT depth bias.
  void SetSsaoSsctDepthBias(float depthBias);

  /// @fn GetSsaoSsctDepthBias
  /// @brief
  /// Get the SSAO SSCT depth bias.
  /// @return
  /// The SSAO SSCT depth bias.
  float GetSsaoSsctDepthBias();

  /// @fn SetSsaoSsctDepthSlopeBias
  /// @brief
  /// Set SSAO SSCT depth slope bias.
  /// @param[in] depthSlopeBias
  /// The SSAO SSCT depth slope bias.
  void SetSsaoSsctDepthSlopeBias(float depthSlopeBias);

  /// @fn GetSsaoSsctDepthSlopeBias
  /// @brief
  /// Get the SSAO SSCT depth slope bias.
  /// @return
  /// The SSAO SSCT depth slope bias.
  float GetSsaoSsctDepthSlopeBias();

  /// @fn SetSsaoSsctSampleCount
  /// @brief
  /// Set SSAO SSCT sample count.
  /// @param[in] sampleCount
  /// The SSAO SSCT sample count.
  void SetSsaoSsctSampleCount(int sampleCount);

  /// @fn GetSsaoSsctSampleCount
  /// @brief
  /// Get the SSAO SSCT sample count.
  /// @return
  /// The SSAO SSCT sample count.
  int GetSsaoSsctSampleCount();

  /// @fn SetSsaoSsctLightDirection
  /// @brief
  /// Set SSAO SSCT light direction.
  /// @param[in] lightDirection
  /// The SSAO SSCT light direction.
  void SetSsaoSsctLightDirection(const float lightDirection[3]);

  /// @fn GetSsaoSsctLightDirection
  /// @brief
  /// Get the SSAO SSCT light direction.
  /// @param[out] lightDirection
  /// The SSAO SSCT light direction.
  void GetSsaoSsctLightDirection(float lightDirection[3]);

  /// @fn SetScreenSpaceReflectionsThickness
  /// @brief
  /// Set screen space reflections thickness.
  /// @param[in] thickness
  /// The screen space reflections thickness.
  void SetScreenSpaceReflectionsThickness(float thickness);

  /// @fn GetScreenSpaceReflectionsThickness
  /// @brief
  /// Get the screen space reflections thickness.
  /// @return
  /// The screen space reflections thickness.
  float GetScreenSpaceReflectionsThickness();

  /// @fn SetScreenSpaceReflectionsBias
  /// @brief
  /// Set screen space reflections bias.
  /// @param[in] bias
  /// The screen space reflections bias.
  void SetScreenSpaceReflectionsBias(float bias);

  /// @fn GetScreenSpaceReflectionsBias
  /// @brief
  /// Get the screen space reflections bias.
  /// @return
  /// The screen space reflections bias.
  float GetScreenSpaceReflectionsBias();

  /// @fn SetScreenSpaceReflectionsMaxDistance
  /// @brief
  /// Set screen space reflections max distance.
  /// @param[in] maxDistance
  /// The screen space reflections max distance.
  void SetScreenSpaceReflectionsMaxDistance(float maxDistance);

  /// @fn GetScreenSpaceReflectionsMaxDistance
  /// @brief
  /// Get the screen space reflections max distance.
  /// @return
  /// The screen space reflections max distance.
  float GetScreenSpaceReflectionsMaxDistance();

  /// @fn SetScreenSpaceReflectionsStride
  /// @brief
  /// Set screen space reflections stride.
  /// @param[in] stride
  /// The screen space reflections stride.
  void SetScreenSpaceReflectionsStride(float stride);

  /// @fn GetScreenSpaceReflectionsStride
  /// @brief
  /// Get the screen space reflections stride.
  /// @return
  /// The screen space reflections stride.
  float GetScreenSpaceReflectionsStride();

  /// @fn SetDynamicResoultionEnabled
  /// @brief
  /// Set dynamic resolution enabled.
  /// @param[in] enabled
  /// The flag to enable dynamic resolution.
  void SetDynamicResoultionEnabled(bool enabled);

  /// @fn IsDynamicResoultionEnabled
  /// @brief
  /// Check if dynamic resolution is enabled.
  /// @return
  /// true if dynamic resolution is enabled, false otherwise.
  bool IsDynamicResoultionEnabled();

  /// @fn SetDynamicResoultionHomogeneousScaling
  /// @brief
  /// Set dynamic resolution homogeneous scaling.
  /// @param[in] homogeneousScaling
  /// The flag to enable dynamic resolution homogeneous scaling.
  void SetDynamicResoultionHomogeneousScaling(bool homogeneousScaling);

  /// @fn IsDynamicResoultionHomogeneousScaling
  /// @brief
  /// Check if dynamic resolution homogeneous scaling is enabled.
  /// @return
  /// true if dynamic resolution homogeneous scaling is enabled, false
  /// otherwise.
  bool IsDynamicResoultionHomogeneousScaling();

  /// @fn SetDynamicResoultionMinScale
  /// @brief
  /// Set dynamic resolution min scale.
  /// @param[in] minScale
  /// The dynamic resolution min scale.
  void SetDynamicResoultionMinScale(float minScale);

  /// @fn GetDynamicResoultionMinScale
  /// @brief
  /// Get the dynamic resolution min scale.
  /// @return
  /// The dynamic resolution min scale.
  float GetDynamicResoultionMinScale();

  /// @fn SetDynamicResoultionMaxScale
  /// @brief
  /// Set dynamic resolution max scale.
  /// @param[in] maxScale
  /// The dynamic resolution max scale.
  void SetDynamicResoultionMaxScale(float maxScale);

  /// @fn GetDynamicResoultionMaxScale
  /// @brief
  /// Get the dynamic resolution max scale.
  /// @return
  /// The dynamic resolution max scale.
  float GetDynamicResoultionMaxScale();

  /// @fn SetDynamicResoultionQuality
  /// @brief
  /// Set dynamic resolution quality.
  /// @param[in] quality
  /// The dynamic resolution quality.
  void SetDynamicResoultionQuality(int quality);

  /// @fn GetDynamicResoultionQuality
  /// @brief
  /// Get the dynamic resolution quality.
  /// @return
  /// The dynamic resolution quality.
  int GetDynamicResoultionQuality();

  /// @fn SetDynamicResoultionSharpness
  /// @brief
  /// Set dynamic resolution sharpness.
  /// @param[in] sharpness
  /// The dynamic resolution sharpness.
  void SetDynamicResoultionSharpness(float sharpness);

  /// @fn GetDynamicResoultionSharpness
  /// @brief
  /// Get the dynamic resolution sharpness.
  /// @return
  /// The dynamic resolution sharpness.
  float GetDynamicResoultionSharpness();

  /// @enum ShadowType
  /// @brief
  /// The shadow type.
  enum class ShadowType : uint8_t {
    PCF,   //!< percentage-closer filtered shadows (default)
    VSM,   //!< variance shadows
    DPCF,  //!< PCF with contact hardening simulation
    PCSS,  //!< PCF with soft shadows and contact hardening
    PCFd,  // for debugging only, don't use.
  };

  /// @fn SetShadowType
  /// @brief
  /// Set shadow type.
  /// @param[in] shadowType
  /// The shadow type.
  void SetShadowType(ShadowType shadowType);

  /// @fn GetShadowType
  /// @brief
  /// Get the shadow type.
  /// @return
  /// The shadow type.
  ShadowType GetShadowType();

  /// @fn SetVsmHighPrecision
  /// @brief
  /// Set VSM high precision.
  /// @param[in] highPrecision
  /// The flag to enable VSM high precision.
  void SetVsmHighPrecision(bool highPrecision);

  /// @fn IsVsmHighPrecision
  /// @brief
  /// Check if VSM high precision is enabled.
  /// @return
  /// true if VSM high precision is enabled, false otherwise.
  bool IsVsmHighPrecision();

  /// @fn SetVsmMsaaSamples
  /// @brief
  /// Set VSM MSAA samples.
  /// @param[in] msaaSamples
  /// The VSM MSAA samples.
  void SetVsmMsaaSamples(int msaaSamples);

  /// @fn GetVsmMsaaSamples
  /// @brief
  /// Get the VSM MSAA samples.
  /// @return
  /// The VSM MSAA samples.
  int GetVsmMsaaSamples();

  /// @fn SetVsmAnisotropy
  /// @brief
  /// Set VSM anisotropy.
  /// @param[in] anisotropy
  /// The VSM anisotropy.
  void SetVsmAnisotropy(int anisotropy);

  /// @fn GetVsmAnisotropy
  /// @brief
  /// Get the VSM anisotropy.
  /// @return
  /// The VSM anisotropy.
  int GetVsmAnisotropy();

  /// @fn SetVsmMipmapping
  /// @brief
  /// Set VSM mipmapping.
  /// @param[in] mipmapping
  /// The flag to enable VSM mipmapping.
  void SetVsmMipmapping(bool mipmapping);

  /// @fn IsVsmMipmapping
  /// @brief
  /// Check if VSM mipmapping is enabled.
  /// @return
  /// true if VSM mipmapping is enabled, false otherwise.
  bool IsVsmMipmapping();

  /// @fn SetSoftShadowPenumbraScale
  /// @brief
  /// Set soft shadow penumbra scale.
  /// @param[in] penumbraScale
  /// The soft shadow penumbra scale.
  void SetSoftShadowPenumbraScale(float penumbraScale);

  /// @fn GetSoftShadowPenumbraScale
  /// @brief
  /// Get the soft shadow penumbra scale.
  /// @return
  /// The soft shadow penumbra scale.
  float GetSoftShadowPenumbraScale();

  /// @fn SetSoftShadowPenumbraRatioScale
  /// @brief
  /// Set soft shadow penumbra ratio scale.
  /// @param[in] penumbraRatioScale
  /// The soft shadow penumbra ratio scale.
  void SetSoftShadowPenumbraRatioScale(float penumbraRatioScale);

  /// @fn GetSoftShadowPenumbraRatioScale
  /// @brief
  /// Get the soft shadow penumbra ratio scale.
  /// @return
  /// The soft shadow penumbra ratio scale.
  float GetSoftShadowPenumbraRatioScale();

  /// @fn SetFogEnabled
  /// @brief
  /// Set fog enabled.
  /// @param[in] enabled
  /// The flag to enable fog.
  void SetFogEnabled(bool enabled);

  /// @fn IsFogEnabled
  /// @brief
  /// Check if fog is enabled.
  /// @return
  /// true if fog is enabled, false otherwise.
  bool IsFogEnabled();

  /// @fn SetFogDistance
  /// @brief
  /// Set fog distance.
  /// @param[in] distance
  /// The fog distance.
  void SetFogDistance(float distance);

  /// @fn GetFogDistance
  /// @brief
  /// Get the fog distance.
  /// @return
  /// The fog distance.
  float GetFogDistance();

  /// @fn SetFogDensity
  /// @brief
  /// Set fog density.
  /// @param[in] density
  /// The fog density.
  void SetFogDensity(float density);

  /// @fn GetFogDensity
  /// @brief
  /// Get the fog density.
  /// @return
  /// The fog density.
  float GetFogDensity();

  /// @fn SetFogHeight
  /// @brief
  /// Set fog height.
  /// @param[in] height
  /// The fog height.
  void SetFogHeight(float height);
  float GetFogHeight();

  /// @fn SetFogHeightFalloff
  /// @brief
  /// Set fog height falloff.
  /// @param[in] heightFalloff
  /// The fog height falloff.
  void SetFogHeightFalloff(float heightFalloff);

  /// @fn GetFogHeightFalloff
  /// @brief
  /// Get the fog height falloff.
  /// @return
  /// The fog height falloff.
  float GetFogHeightFalloff();

  /// @fn SetFogInScatteringStart
  /// @brief
  /// Set fog in scattering start.
  /// @param[in] inScatteringStart
  /// The fog in scattering start.
  void SetFogInScatteringStart(float inScatteringStart);

  /// @fn GetFogInScatteringStart
  /// @brief
  /// Get the fog in scattering start.
  /// @return
  /// The fog in scattering start.
  float GetFogInScatteringStart();

  /// @fn SetFogInScatteringSize
  /// @brief
  /// Set fog in scattering size.
  /// @param[in] inScatteringSize
  /// The fog in scattering size.
  void SetFogInScatteringSize(float inScatteringSize);

  /// @fn GetFogInScatteringSize
  /// @brief
  /// Get the fog in scattering size.
  /// @return
  /// The fog in scattering size.
  float GetFogInScatteringSize();

  /// @fn SetFogExcludeSkybox
  /// @brief
  /// Set fog exclude skybox.
  /// @param[in] excludeSkybox
  /// The flag to exclude skybox from fog.
  void SetFogExcludeSkybox(bool excludeSkybox);

  /// @fn IsFogExcludeSkybox
  /// @brief
  /// Check if skybox is excluded from fog.
  /// @return
  /// true if skybox is excluded from fog, false otherwise.
  bool IsFogExcludeSkybox();

  /// @enum FogColorSource
  /// @brief
  /// The fog color source.
  enum class FogColorSource : uint8_t { CONSTANT, IBL, SKYBOX };

  /// @fn SetFogColorSource
  /// @brief
  /// Set fog color source.
  /// @param[in] fogColorSource
  /// The fog color source.
  void SetFogColorSource(FogColorSource fogColorSource);

  /// @fn GetFogColorSource
  /// @brief
  /// Get the fog color source.
  /// @return
  /// The fog color source.
  FogColorSource GetFogColorSource();

  /// @fn SetFogColor
  /// @brief
  /// Set fog color.
  /// @param[in] color
  /// The fog color.
  void SetFogColor(const float color[3]);

  /// @fn GetFogColor
  /// @brief
  /// Get the fog color.
  /// @param[out] color
  /// The fog color.
  void GetFogColor(float color[3]);

  /// @fn SetDofEnabled
  /// @brief
  /// Set DoF enabled.
  /// @param[in] enabled
  /// The flag to enable DoF.
  void SetDofEnabled(bool enabled);

  /// @fn IsDofEnabled
  /// @brief
  /// Check if DoF is enabled.
  /// @return
  /// true if DoF is enabled, false otherwise.
  bool IsDofEnabled();

  /// @fn SetDofCocScale
  /// @brief
  /// Set the DoF CoC scale.
  /// @param[in] cocScale
  /// The DoF CoC scale.
  void SetDofCocScale(float cocScale);

  /// @fn GetDofCocScale
  /// @brief
  /// Get the DoF CoC scale.
  /// @return
  /// The DoF CoC scale.
  float GetDofCocScale();

  /// @fn SetDofCocAspectRatio
  /// @brief
  /// Set the DoF CoC aspect ratio.
  /// @param[in] cocAspectRatio
  /// The DoF CoC aspect ratio.
  void SetDofCocAspectRatio(float cocAspectRatio);

  /// @fn GetDofCocAspectRatio
  /// @brief
  /// Get the DoF CoC aspect ratio.
  /// @return
  /// The DoF CoC aspect ratio.
  float GetDofCocAspectRatio();

  /// @fn SetDofRingCount
  /// @brief
  /// Set the DoF ring count.
  /// @param[in] dofRingCount
  /// The DoF ring count.
  void SetDofRingCount(int dofRingCount);

  /// @fn GetDofRingCount
  /// @brief
  /// Get the DoF ring count.
  /// @return
  /// The DoF ring count.
  int GetDofRingCount();

  /// @fn SetDofMaxCoc
  /// @brief
  /// Set the DoF max CoC.
  /// @param[in] maxCoc
  /// The DoF max CoC.
  void SetDofMaxCoc(int maxCoc);

  /// @fn GetDofMaxCoc
  /// @brief
  /// Get the DoF max CoC.
  /// @return
  /// The DoF max CoC.
  int GetDofMaxCoc();

  /// @fn SetDofNativeResolution
  /// @brief
  /// Set DoF native resolution.
  /// @param[in] nativeResolution
  /// The flag to enable DoF native resolution.
  void SetDofNativeResolution(bool nativeResolution);

  /// @fn IsDofNativeResolution
  /// @brief
  /// Check if DoF native resolution is enabled.
  /// @return
  /// true if DoF native resolution is enabled, false otherwise.
  bool IsDofNativeResolution();

  /// @fn SetDofMedian
  /// @brief
  /// Set DoF median.
  /// @param[in] dofMedian
  /// The flag to enable DoF median.
  void SetDofMedian(bool dofMedian);

  /// @fn IsDofMedian
  /// @brief
  /// Check if DoF median is enabled.
  /// @return
  /// true if DoF median is enabled, false otherwise.
  bool IsDofMedian();

  /// @fn SetVignetteEnabled
  /// @brief
  /// Set vignette enabled.
  /// @param[in] enabled
  /// The flag to enable vignette.
  void SetVignetteEnabled(bool enabled);

  /// @fn IsVignetteEnabled
  /// @brief
  /// Check if vignette is enabled.
  /// @return
  /// true if vignette is enabled, false otherwise.
  bool IsVignetteEnabled();

  /// @fn SetVignetteMidPoint
  /// @brief
  /// Set vignette mid point.
  /// @param[in] midPoint
  /// The vignette mid point.
  void SetVignetteMidPoint(float midPoint);

  /// @fn GetVignetteMidPoint
  /// @brief
  /// Get the vignette mid point.
  /// @return
  /// The vignette mid point.
  float GetVignetteMidPoint();

  /// @fn SetVignetteRoundness
  /// @brief
  /// Set vignette roundness.
  /// @param[in] roundness
  /// The vignette roundness.
  void SetVignetteRoundness(float roundness);

  /// @fn GetVignetteRoundness
  /// @brief
  /// Get the vignette roundness.
  /// @return
  /// The vignette roundness.
  float GetVignetteRoundness();

  /// @fn SetVignetteFeather
  /// @brief
  /// Set vignette feather.
  /// @param[in] feather
  /// The vignette feather.
  void SetVignetteFeather(float feather);

  /// @fn GetVignetteFeather
  /// @brief
  /// Get the vignette feather.
  /// @return
  /// The vignette feather.
  float GetVignetteFeather();

  /// @fn SetVignetteColor
  /// @brief
  /// Set vignette color.
  /// @param[in] color
  /// The vignette color.
  void SetVignetteColor(const float color[3]);

  /// @fn GetVignetteColor
  /// @brief
  /// Get the vignette color.
  /// @param[out] color
  /// The vignette color.
  void GetVignetteColor(float color[3]);

  /// @struct ClearOptions
  /// @brief
  /// The clear options.
  struct ClearOptions {
    /// @var clearColor
    /// @brief
    /// The clear color.
    float clearColor[4] = {};

    /// @var clearStencil
    /// @brief
    /// The clear stencil.
    uint8_t clearStencil = 0u;

    /// @var clear
    /// @brief
    /// The flag to enable clear.
    bool clear = false;

    /// @var discard
    /// @brief
    /// The flag to enable discard.
    bool discard = true;
  };

  /// @fn SetClearOptions
  /// @brief
  /// Set clear options.
  /// @param[in] clearOptions
  /// The clear options.
  void SetClearOptions(const ClearOptions& clearOptions);

  /// @fn GetClearOptions
  /// @brief
  /// Get the clear options.
  /// @param[out] clearOptions
  /// The clear options.
  void GetClearOptions(ClearOptions& clearOptions);

  /// @fn Render
  /// @brief
  /// Render the scene with the camera.
  /// @param[in] vidScene
  /// The unique identifier of the scene.
  /// @param[in] vidCam
  /// The unique identifier of the camera.
  /// @return
  /// The result of the operation.
  VZRESULT Render(const VID vidScene, const VID vidCam);

  /// @fn Render
  /// @brief
  /// Render the scene with the camera.
  /// @param[in] scene
  /// The scene.
  /// @param[in] camera
  /// The camera.
  /// @return
  /// The result of the operation.
  VZRESULT Render(const VzBaseComp* scene, const VzBaseComp* camera) {
    return Render(scene->GetVID(), camera->GetVID());
  }
};
}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZRENDERER_H_
