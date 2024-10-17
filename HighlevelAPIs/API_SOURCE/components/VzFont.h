///
/// @file      VzFont.h
/// @brief     The header file for the VzFont class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZFONT_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZFONT_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzFont
/// @brief
/// The class for the font.
struct API_EXPORT VzFont : VzResource {
  /// @brief
  /// Constructor for the VzFont class.
  /// @param[in] vid
  /// The unique identifier for the font.
  /// @param[in] originFrom
  /// The name of the function that created the font.
  VzFont(const VID vid, const std::string& originFrom)
      : VzResource(vid, originFrom, "VzFont", RES_COMPONENT_TYPE::FONT) {}

  /// @fn ReadFont
  /// @brief
  /// Read the font.
  /// @param[in] fileName
  /// The file name.
  /// @param[in] fontSize
  /// The font size.
  /// @return
  /// true if the font is read successfully, false otherwise.
  bool ReadFont(const std::string& fileName, const uint32_t fontSize = 10);

  /// @fn GetFontFileName
  /// @brief
  /// Get the font file name.
  /// @return
  /// The font file name.
  std::string GetFontFileName();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZFONT_H_
