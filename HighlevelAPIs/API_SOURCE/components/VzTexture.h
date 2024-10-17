///
/// @file      VzTexture.h
/// @brief     The header file for the VzTexture class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZTEXTURE_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZTEXTURE_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzTexture
/// @brief
/// The class for the texture.
struct API_EXPORT VzTexture : VzResource {
  /// @brief
  /// Constructor for the VzTexture class.
  /// @param[in] vid
  /// The unique identifier for the texture.
  /// @param[in] originFrom
  /// The name of the function that created the texture.
  VzTexture(const VID vid, const std::string& originFrom)
      : VzResource(vid, originFrom, "VzTexture", RES_COMPONENT_TYPE::TEXTURE) {}

  /// @fn ReadImage
  /// @brief
  /// Read the image.
  /// @param[in] fileName
  /// The file name.
  /// @param[in] isLinear
  /// The flag to indicate whether the image is linear.
  /// @param[in] generateMIPs
  /// The flag to indicate whether to generate MIPs.
  /// @return
  /// true if the image is read successfully, false otherwise.
  bool ReadImage(const std::string& fileName, const bool isLinear = true,
                 const bool generateMIPs = true);

  /// @fn GetImageFileName
  /// @brief
  /// Get the image file name.
  /// @return
  /// The image file name.
  std::string GetImageFileName();

  /// @fn SetMinFilter
  /// @brief
  /// Set the min filter.
  /// @param[in] filter
  /// The min filter.
  void SetMinFilter(const SamplerMinFilter filter);

  /// @fn SetMagFilter
  /// @brief
  /// Set the mag filter.
  /// @param[in] filter
  /// The mag filter.
  void SetMagFilter(const SamplerMagFilter filter);

  /// @fn SetWrapModeS
  /// @brief
  /// Set the wrap mode for the S coordinate.
  /// @param[in] mode
  /// The wrap mode for the S coordinate.
  void SetWrapModeS(const SamplerWrapMode mode);

  /// @fn SetWrapModeT
  /// @brief
  /// Set the wrap mode for the T coordinate.
  /// @param[in] mode
  /// The wrap mode for the T coordinate.
  void SetWrapModeT(const SamplerWrapMode mode);

  /// @fn GenerateMIPs
  /// @brief
  /// Generate MIPs.
  /// @return
  /// true if the MIPs are generated successfully, false otherwise.
  bool GenerateMIPs();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZTEXTURE_H_
