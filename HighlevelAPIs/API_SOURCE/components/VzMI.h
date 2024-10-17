///
/// @file      VzMI.h
/// @brief     The header file for the VzMI class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMI_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMI_H_

#include "../VizComponentAPIs.h"
#include "VzMaterial.h"

namespace vzm {

/// @struct VzMI
/// @brief
/// The class for the material instance.
struct API_EXPORT VzMI : VzResource {
  /// @brief
  /// Constructor for the VzMI class.
  VzMI(const VID vid, const std::string& originFrom)
      : VzResource(vid, originFrom, "VzMI",
                   RES_COMPONENT_TYPE::MATERIALINSTANCE) {}

  /// @enum TransparencyMode
  /// @brief
  /// The transparency mode.
  enum class TransparencyMode : uint8_t {
    DEFAULT = 0,
    TWO_PASSES_ONE_SIDE,
    TWO_PASSES_TWO_SIDES
  };

  /// @fn SetDoubleSided
  /// @brief
  /// Set the double-sided flag.
  /// @param[in] doubleSided
  /// The double-sided flag.
  void SetDoubleSided(const bool doubleSided);

  /// @fn IsDoubleSided
  /// @brief
  /// Check if the material instance is double-sided.
  /// @return
  /// true if the material instance is double-sided, false otherwise.
  bool IsDoubleSided() const;

  /// @fn SetTransparencyMode
  /// @brief
  /// Set the transparency mode.
  /// @param[in] tMode
  /// The transparency mode.
  void SetTransparencyMode(const TransparencyMode tMode);

  /// @fn GetTransparencyMode
  /// @brief
  /// Get the transparency mode.
  /// @return
  /// The transparency mode.
  TransparencyMode GetTransparencyMode() const;

  /// @fn SetParameter
  /// @brief
  /// Set the parameter.
  /// @param[in] name
  /// The name of the parameter.
  /// @param[in] vType
  /// The uniform type.
  /// @param[in] v
  /// The value of the parameter.
  /// @return
  /// true if the parameter is set successfully, false otherwise.
  bool SetParameter(const std::string& name, const vzm::UniformType vType,
                    const void* v);

  /// @fn SetParameter
  /// @brief
  /// Set the parameter.
  /// @param[in] name
  /// The name of the parameter.
  /// @param[in] vType
  /// The RGB type.
  /// @param[in] v
  /// The value of the parameter.
  /// @return
  /// true if the parameter is set successfully, false otherwise.
  bool SetParameter(const std::string& name, const vzm::RgbType vType,
                    const float* v);

  /// @fn SetParameter
  /// @brief
  /// Set the parameter.
  /// @param[in] name
  /// The name of the parameter.
  /// @param[in] vType
  /// The RGBA type.
  /// @param[in] v
  /// The value of the parameter.
  /// @return
  /// true if the parameter is set successfully, false otherwise.
  bool SetParameter(const std::string& name, const vzm::RgbaType vType,
                    const float* v);

  /// @fn GetParameter
  /// @brief
  /// Get the parameter.
  /// @param[in] name
  /// The name of the parameter.
  /// @param[in] vType
  /// The uniform type.
  /// @param[out] v
  /// The value of the parameter.
  /// @return
  /// true if the parameter is retrieved successfully, false otherwise.
  bool GetParameter(const std::string& name, const vzm::UniformType vType,
                    const void* v);

  /// @fn GetTexture
  /// @brief
  /// Get the texture.
  /// @param[in] name
  /// The name of the texture.
  /// @return
  /// The unique identifier for the texture.
  VID GetTexture(const std::string& name);

  /// @fn SetTexture
  /// @brief
  /// Set the texture.
  /// @param[in] name
  /// The name of the texture.
  /// @param[in] vidTexture
  /// The unique identifier for the texture.
  /// @param[in] retainSampler
  /// The flag to retain the sampler.
  /// @return
  /// true if the texture is set successfully, false otherwise.
  bool SetTexture(const std::string& name, const VID vidTexture,
                  const bool retainSampler = true);

  /// @fn GetUvTransform
  /// @brief
  /// Get the UV transform.
  /// @param[in] name
  /// The name of the UV transform.
  /// @param[out] offset
  /// The offset.
  /// @param[out] rotation
  /// The rotation.
  /// @param[out] scale
  /// The scale.
  /// @return
  /// true if the UV transform is retrieved successfully, false otherwise.
  bool GetUvTransform(const std::string& name, float offset[2], float& rotation,
                      float scale[2]) const;

  /// @fn SetUvTransform
  /// @brief
  /// Set the UV transform.
  /// @param[in] name
  /// The name of the UV transform.
  /// @param[in] offset
  /// The offset.
  /// @param[in] rotation
  /// The rotation.
  /// @param[in] scale
  /// The scale.
  /// @return
  /// true if the UV transform is set successfully, false otherwise.
  bool SetUvTransform(const std::string& name, const float offset[2],
                      const float rotation, const float scale[2]);

  /// @fn GetMaterial
  /// @brief
  /// Get the material.
  /// @return
  /// The unique identifier for the material.
  VID GetMaterial();

  /// @fn SetMaterial
  /// @brief
  /// Set the material.
  /// @param[in] vidMaterial
  /// The unique identifier for the material.
  /// @return
  /// true if the material is set successfully, false otherwise.
  bool SetMaterial(const VID vidMaterial);
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMI_H_
