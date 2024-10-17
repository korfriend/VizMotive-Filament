///
/// @file      VzLight.h
/// @brief     The header file for the VzLight class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZLIGHT_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZLIGHT_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzBaseLight
/// @brief
/// The base class for the light.
struct API_EXPORT VzBaseLight : VzSceneComp {
  /// @brief
  /// Constructor for the VzBaseLight class.
  /// @param[in] vid
  /// The unique identifier for the light.
  /// @param[in] originFrom
  /// The name of the function that created the light.
  VzBaseLight(const VID vid, const std::string& originFrom,
              const std::string& typeName,
              const SCENE_COMPONENT_TYPE scenecompType)
      : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

  /// @fn SetLightChannel
  /// @brief
  /// Set the light channel.
  /// @param[in] channel
  /// The channel.
  /// @param[in] enable
  /// The enable flag.
  void SetLightChannel(unsigned int channel, bool enable = true);

  /// @fn GetLightChannel
  /// @brief
  /// Get the light channel.
  /// @param[in] channel
  /// The channel.
  /// @return
  /// true if the light channel is enabled, false otherwise.
  bool GetLightChannel(unsigned int channel) const;

  /// @fn SetPosition
  /// @brief
  /// Set the light position.
  /// @param[in] position
  /// The position.
  void SetPosition(const float position[3]);

  /// @fn GetPosition
  /// @brief
  /// Get the light position.
  /// @param[out] position
  /// The position.
  void GetPosition(float position[3]);

  /// @fn SetDirection
  /// @brief
  /// Set the light direction.
  /// @param[in] direction
  /// The direction.
  void SetDirection(const float direction[3]);

  /// @fn GetDirection
  /// @brief
  /// Get the light direction.
  /// @param[out] direction
  /// The direction.
  void GetDirection(float direction[3]);

  /// @fn SetColor
  /// @brief
  /// Set the light color.
  /// @param[in] color
  /// The color.
  void SetColor(const float color[3]);

  /// @fn GetColor
  /// @brief
  /// Get the light color.
  /// @param[out] color
  /// The color.
  void GetColor(float color[3]);

  /// @fn SetIntensity
  /// @brief
  /// Set the light intensity.
  /// @param[in] intensity
  /// The intensity.
  void SetIntensity(const float intensity);

  /// @fn GetIntensity
  /// @brief
  /// Get the light intensity.
  /// @return
  /// The intensity.
  float GetIntensity() const;

  /// @struct ShadowOptions
  /// @brief
  /// The shadow options.
  struct ShadowOptions {
    /// @var mapSize
    /// @brief
    /// The map size.
    uint32_t mapSize = 1024;

    /// @var shadowCascades
    /// @brief
    /// The shadow cascades.
    uint8_t shadowCascades = 1;

    /// @var cascadeSplitPositions
    /// @brief
    /// The cascade split positions.
    float cascadeSplitPositions[3] = {0.125f, 0.25f, 0.50f};

    /// @var constantBias
    /// @brief
    /// The constant bias.
    float constantBias = 0.001f;

    /// @var normalBias
    /// @brief
    /// The normal bias.
    float normalBias = 1.0f;

    /// @var shadowFar
    /// @brief
    /// The shadow far.
    float shadowFar = 0.0f;

    /// @var shadowNearHint
    /// @brief
    /// The shadow near hint.
    float shadowNearHint = 1.0f;

    /// @var shadowFarHint
    /// @brief
    /// The shadow far hint.
    float shadowFarHint = 100.0f;

    /// @var stable
    /// @brief
    /// The stable flag.
    bool stable = false;

    /// @var lispsm
    /// @brief
    /// The lispsm flag.
    bool lispsm = true;

    /// @var polygonOffsetConstant
    /// @brief
    /// The polygon offset constant.
    float polygonOffsetConstant = 0.5f;

    /// @var polygonOffsetSlope
    /// @brief
    /// The polygon offset slope.
    float polygonOffsetSlope = 2.0f;

    /// @var screenSpaceContactShadows
    /// @brief
    /// The screen space contact shadows flag.
    bool screenSpaceContactShadows = false;

    /// @var stepCount
    /// @brief
    /// The step count.
    uint8_t stepCount = 8;

    /// @var maxShadowDistance
    /// @brief
    /// The maximum shadow distance.
    float maxShadowDistance = 0.3f;

    /// @struct Vsm
    struct Vsm {
      /// @var elvsm
      /// @brief
      /// The elvsm flag.
      bool elvsm = false;

      /// @var blurWidth
      /// @brief
      /// The blur width.
      float blurWidth = 0.0f;
    };

    /// @var vsm
    /// @brief
    /// The vsm.
    Vsm vsm;

    /// @var shadowBulbRadius
    /// @brief
    /// The shadow bulb radius.
    float shadowBulbRadius = 0.02f;

    /// @var transform
    /// @brief
    /// The transform.
    float transform[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  };

  /// @fn SetShadowOptions
  /// @brief
  /// Set the shadow options.
  /// @param[in] options
  /// The shadow options.
  void SetShadowOptions(ShadowOptions const& options);

  /// @fn GetShadowOptions
  /// @brief
  /// Get the shadow options.
  /// @return
  /// The shadow options.
  ShadowOptions const* GetShadowOptions() const;

  /// @fn SetShadowCaster
  /// @brief
  /// Set the shadow caster.
  /// @param[in] shadowCaster
  /// The shadow caster flag.
  void SetShadowCaster(bool shadowCaster);

  /// @fn IsShadowCaster
  /// @brief
  /// Check if the light is a shadow caster.
  /// @return
  /// true if the light is a shadow caster, false otherwise.
  bool IsShadowCaster() const;
};

/// @struct VzBaseSpotLight
/// @brief
/// The base class for the spot light.
struct API_EXPORT VzBaseSpotLight {
 private:
  /// @cond internal
  VzBaseLight* baseLight_;
  /// @endcond

 public:
  /// @brief
  /// Constructor for the VzBaseSpotLight class.
  /// @param[in] baseLight
  /// The base light.
  VzBaseSpotLight(VzBaseLight* baseLight) : baseLight_(baseLight) {}

  /// @fn SetSpotLightCone
  /// @brief
  /// Set the spot light cone.
  /// @param[in] inner
  /// The inner cone.
  /// @param[in] outer
  /// The outer cone.
  void SetSpotLightCone(const float inner, const float outer);

  /// @fn GetSpotLightOuterCone
  /// @brief
  /// Get the spot light outer cone.
  /// @return
  /// The outer cone.
  float GetSpotLightOuterCone() const;

  /// @fn GetSpotLightInnerCone
  /// @brief
  /// Get the spot light inner cone.
  /// @return
  /// The inner cone.
  float GetSpotLightInnerCone() const;

  /// @fn SetFalloff
  /// @brief
  /// Set the falloff.
  /// @param[in] radius
  /// The radius.
  void SetFalloff(const float radius);

  /// @fn GetFalloff
  /// @brief
  /// Get the falloff.
  /// @return
  /// The radius.
  float GetFalloff() const;
};

/// @struct VzSpotLight
/// @brief
/// The class for the spot light.
struct API_EXPORT VzSpotLight : VzBaseLight, VzBaseSpotLight {
  /// @brief
  /// Constructor for the VzSpotLight class.
  /// @param[in] vid
  /// The unique identifier for the spot light.
  /// @param[in] originFrom
  /// The name of the function that created the spot light.
  VzSpotLight(const VID vid, const std::string& originFrom)
      : VzBaseLight(vid, originFrom, "VzSpotLight",
                    SCENE_COMPONENT_TYPE::LIGHT_SPOT),
        VzBaseSpotLight(this) {}
};

/// @struct VzFocusedSpotLight
/// @brief
/// The class for the focused spot light.
struct API_EXPORT VzFocusedSpotLight : VzBaseLight, VzBaseSpotLight {
  /// @brief
  /// Constructor for the VzFocusedSpotLight class.
  /// @param[in] vid
  /// The unique identifier for the focused spot light.
  /// @param[in] originFrom
  /// The name of the function that created the focused spot light.
  VzFocusedSpotLight(const VID vid, const std::string& originFrom)
      : VzBaseLight(vid, originFrom, "VzFocusedSpotLight",
                    SCENE_COMPONENT_TYPE::LIGHT_FOCUSED_SPOT),
        VzBaseSpotLight(this) {}
};

/// @struct VzSunLight
/// @brief
/// The class for the sun light.
struct API_EXPORT VzSunLight : VzBaseLight {
  /// @brief
  /// Constructor for the VzSunLight class.
  /// @param[in] vid
  /// The unique identifier for the sun light.
  /// @param[in] originFrom
  /// The name of the function that created the sun light.
  VzSunLight(const VID vid, const std::string& originFrom)
      : VzBaseLight(vid, originFrom, "VzSunLight",
                    SCENE_COMPONENT_TYPE::LIGHT_SUN) {}

  /// @fn SetSunAngularRadius
  /// @brief
  /// Set the sun angular radius.
  /// @param[in] angularRadius
  /// The angular radius.
  void SetSunAngularRadius(const float angularRadius);

  /// @fn GetSunAngularRadius
  /// @brief
  /// Get the sun angular radius.
  /// @return
  /// The angular radius.
  float GetSunAngularRadius() const;

  /// @fn SetSunHaloSize
  /// @brief
  /// Set the sun halo size.
  /// @param[in] haloSize
  /// The halo size.
  void SetSunHaloSize(const float haloSize);

  /// @fn GetSunHaloSize
  /// @brief
  /// Get the sun halo size.
  /// @return
  /// The halo size.
  float GetSunHaloSize() const;

  /// @fn SetSunHaloFalloff
  /// @brief
  /// Set the sun halo falloff.
  /// @param[in] haloFalloff
  /// The halo falloff.
  /// @return
  /// The halo falloff.
  void SetSunHaloFalloff(const float haloFalloff);

  /// @fn GetSunHaloFalloff
  /// @brief
  /// Get the sun halo falloff.
  /// @return
  /// The halo falloff.
  float GetSunHaloFalloff() const;
};

/// @struct VzDirectionalLight
/// @brief
/// The class for the directional light.
struct API_EXPORT VzDirectionalLight : VzBaseLight {
  /// @brief
  /// Constructor for the VzDirectionalLight class.
  VzDirectionalLight(const VID vid, const std::string& originFrom)
      : VzBaseLight(vid, originFrom, "VzDirectionalLight",
                    SCENE_COMPONENT_TYPE::LIGHT_DIRECTIONAL) {}
};

/// @struct VzPointLight
/// @brief
/// The class for the point light.
struct API_EXPORT VzPointLight : VzBaseLight {
  /// @brief
  /// Constructor for the VzPointLight class.
  /// @param[in] vid
  /// The unique identifier for the point light.
  /// @param[in] originFrom
  /// The name of the function that created the point light.
  VzPointLight(const VID vid, const std::string& originFrom)
      : VzBaseLight(vid, originFrom, "VzPointLight",
                    SCENE_COMPONENT_TYPE::LIGHT_POINT) {}

  /// @fn SetFalloff
  /// @brief
  /// Set the falloff.
  void SetFalloff(const float radius);

  /// @fn GetFalloff
  /// @brief
  /// Get the falloff.
  float GetFalloff() const;
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZLIGHT_H_
