///
/// @file      VzScene.h
/// @brief     The header file for the VzScene class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSCENE_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSCENE_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzScene
/// @brief
/// The class for the scene.
struct API_EXPORT VzScene : VzBaseComp {
 public:
  /// @brief
  /// Constructor for the VzScene class.
  /// @param[in] vid
  /// The unique identifier for the scene.
  /// @param[in] originFrom
  /// The name of the function that created the scene.
  VzScene(const VID vid, const std::string& originFrom)
      : VzBaseComp(vid, originFrom, "VzScene") {}

  /// @fn GetSceneCompChildren
  /// @brief
  /// Get the scene component children.
  /// @return
  /// The scene component children.
  std::vector<VID> GetSceneCompChildren();

  /// @fn LoadIBL
  /// @brief
  /// Load the IBL.
  /// @param[in] iblPath
  /// The IBL path.
  /// @return
  /// true if the IBL is loaded successfully, false otherwise.
  bool LoadIBL(const std::string& iblPath);

  /// @fn GetIBLIntensity
  /// @brief
  /// Get the IBL intensity.
  /// @return
  /// The IBL intensity.
  float GetIBLIntensity();

  /// @fn GetIBLRotation
  /// @brief
  /// Get the IBL rotation.
  /// @return
  /// The IBL rotation.
  float GetIBLRotation();

  /// @fn SetIBLIntensity
  /// @brief
  /// Set the IBL intensity.
  /// @param[in] intensity
  /// The IBL intensity.
  void SetIBLIntensity(float intensity);

  /// @fn SetIBLRotation
  /// @brief
  /// Set the IBL rotation.
  /// @param[in] rotation
  /// The IBL rotation.
  void SetIBLRotation(float rotation);

  /// @fn SetSkyboxVisibleLayerMask
  /// @brief
  /// Set the skybox visible layer mask.
  /// @param[in] layerBits
  /// The layer bits.
  /// @param[in] maskBits
  /// The mask bits.
  void SetSkyboxVisibleLayerMask(const uint8_t layerBits = 0x7,
                                 const uint8_t maskBits = 0x4);

  /// @fn SetLightmapVisibleLayerMask
  /// @brief
  /// Set the lightmap visible layer mask.
  /// @param[in] layerBits
  /// The layer bits.
  /// @param[in] maskBits
  /// The mask bits.
  void SetLightmapVisibleLayerMask(const uint8_t layerBits = 0x3,
                                   const uint8_t maskBits = 0x2);
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSCENE_H_
