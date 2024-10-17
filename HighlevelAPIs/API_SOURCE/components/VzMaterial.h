///
/// @file      VzMaterial.h
/// @brief     The header file for the VzMaterial class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMATERIAL_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMATERIAL_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzMaterial
/// @brief
/// The class for the material.
struct API_EXPORT VzMaterial : VzResource {
  /// @brief
  /// Constructor for the VzMaterial class.
  /// @param[in] vid
  /// The unique identifier for the material.
  /// @param[in] originFrom
  /// The name of the function that created the material.
  VzMaterial(const VID vid, const std::string& originFrom)
      : VzResource(vid, originFrom, "VzMaterial",
                   RES_COMPONENT_TYPE::MATERIAL) {}

  /// @enum LightingModel
  /// @brief
  /// The lighting model.
  enum class LightingModel : uint8_t {
    UNLIT,                //!< no lighting applied, emissive possible
    LIT,                  //!< default, standard lighting
    SUBSURFACE,           //!< subsurface lighting model
    CLOTH,                //!< cloth lighting model
    SPECULAR_GLOSSINESS,  //!< legacy lighting model
  };

  /// @enum ParameterInfo
  /// @brief
  /// The parameter information.
  struct ParameterInfo {
    //! Name of the parameter.
    const char* name;
    //! Whether the parameter is a sampler (texture).
    bool isSampler;
    //! Whether the parameter is a subpass type.
    bool isSubpass;
    union {
      //! Type of the parameter if the parameter is not a sampler.
      UniformType type;
      //! Type of the parameter if the parameter is a sampler.
      SamplerType samplerType;
      //! Type of the parameter if the parameter is a subpass.
      SubpassType subpassType;
    };
    //! Size of the parameter when the parameter is an array.
    uint32_t count;
    //! Requested precision of the parameter.
    Precision precision;
  };

  /// @enum AlphaMode
  /// @brief
  /// The alpha mode.
  enum class AlphaMode : uint8_t { OPAQUE, MASK, BLEND };

  /// @struct MaterialKey
  /// @brief
  /// The material key.
  struct alignas(4) MaterialKey {
    // -- 32 bit boundary --
    bool doubleSided : 1;
    bool unlit : 1;
    bool hasVertexColors : 1;
    bool hasBaseColorTexture : 1;
    bool hasNormalTexture : 1;
    bool hasOcclusionTexture : 1;
    bool hasEmissiveTexture : 1;
    bool useSpecularGlossiness : 1;
    AlphaMode alphaMode : 4;
    bool enableDiagnostics : 4;
    union {
      struct {
        bool hasMetallicRoughnessTexture : 1;
        uint8_t metallicRoughnessUV : 7;
      };
      struct {
        bool hasSpecularGlossinessTexture : 1;
        uint8_t specularGlossinessUV : 7;
      };
    };
    uint8_t baseColorUV;
    // -- 32 bit boundary --
    bool hasClearCoatTexture : 1;
    uint8_t clearCoatUV : 7;
    bool hasClearCoatRoughnessTexture : 1;
    uint8_t clearCoatRoughnessUV : 7;
    bool hasClearCoatNormalTexture : 1;
    uint8_t clearCoatNormalUV : 7;
    bool hasClearCoat : 1;
    bool hasTransmission : 1;
    bool hasTextureTransforms : 6;
    // -- 32 bit boundary --
    uint8_t emissiveUV;
    uint8_t aoUV;
    uint8_t normalUV;
    bool hasTransmissionTexture : 1;
    uint8_t transmissionUV : 7;
    // -- 32 bit boundary --
    bool hasSheenColorTexture : 1;
    uint8_t sheenColorUV : 7;
    bool hasSheenRoughnessTexture : 1;
    uint8_t sheenRoughnessUV : 7;
    bool hasVolumeThicknessTexture : 1;
    uint8_t volumeThicknessUV : 7;
    bool hasSheen : 1;
    bool hasIOR : 1;
    bool hasVolume : 1;
    bool hasSpecular : 1;
    bool hasSpecularTexture : 1;
    bool hasSpecularColorTexture : 1;
    bool padding : 2;
    // -- 32 bit boundary --
    uint8_t specularTextureUV;
    uint8_t specularColorTextureUV;
    uint16_t padding2;
  };

  /// @fn IsStandardMaterial
  /// @brief
  /// Check if the material is a standard material.
  /// @return
  /// true if the material is a standard material, false otherwise.
  bool IsStandardMaterial() const;

  /// @fn SetLightingModel
  /// @brief
  /// Set the lighting model.
  /// @param[in] model
  /// The lighting model.
  void SetLightingModel(const LightingModel model);

  /// @fn GetLightingModel
  /// @brief
  /// Get the lighting model.
  /// @return
  /// The lighting model.
  LightingModel GetLightingModel() const;

  /// @fn GetAllowedParameters
  /// @brief
  /// Get the allowed parameters.
  /// @param[out] paramters
  /// The parameters.
  /// @return
  /// The number of parameters.
  size_t GetAllowedParameters(std::map<std::string, ParameterInfo>& paramters);

  /// @fn GetStandardMaterialKey
  /// @brief
  /// Get the standard material key.
  /// @param[out] materialKey
  /// The material key.
  /// @return
  /// true if the material key is retrieved successfully, false otherwise.
  bool GetStandardMaterialKey(MaterialKey& materialKey);

  /// @fn SetStandardMaterialByKey
  /// @brief
  /// Set the standard material by the key.
  /// @param[in] materialKey
  /// The material key.
  /// @return
  /// true if the standard material is set successfully, false otherwise.
  bool SetStandardMaterialByKey(const MaterialKey& materialKey);
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZMATERIAL_H_
