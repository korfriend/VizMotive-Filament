///
/// @file      VzGeometry.h
/// @brief     The header file for the VzGeometry class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZGEOMETRY_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZGEOMETRY_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzGeometry
/// @brief
/// The class for the geometry.
struct API_EXPORT VzGeometry : VzResource {
  /// @brief
  /// Constructor for the VzGeometry class.
  /// @param[in] vid
  /// The unique identifier for the geometry.
  /// @param[in] originFrom
  /// The name of the function that created the geometry.
  VzGeometry(const VID vid, const std::string& originFrom)
      : VzResource(vid, originFrom, "VzGeometry",
                   RES_COMPONENT_TYPE::GEOMATRY) {}
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZGEOMETRY_H_
