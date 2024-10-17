///
/// @file      VzSkeleton.h
/// @brief     The header file for the VzSkeleton class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSKELETON_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSKELETON_H_

#include "../VizComponentAPIs.h"

namespace vzm {
/// @struct VzSkeleton
/// @brief
/// The class for the skeleton.
struct API_EXPORT VzSkeleton : VzBaseComp {
  /// @brief
  /// Constructor for the VzSkeleton class.
  /// @param[in] vid
  /// The unique identifier for the skeleton.
  /// @param[in] originFrom
  /// The name of the function that created the skeleton.
  VzSkeleton(const VID vid, const std::string& originFrom)
      : VzBaseComp(vid, originFrom, "VzSkeleton") {}

  using BoneVID = VID;

  /// @fn GetBones
  /// @brief
  /// Get the bones.
  /// @return
  /// The bones.
  std::vector<BoneVID> GetBones();

  /// @fn GetChildren
  /// @brief
  /// Get the children.
  /// @return
  /// The children.
  std::vector<BoneVID> GetChildren();

  /// @fn GetParent
  /// @brief
  /// Get the parent.
  /// @return
  /// The parent.
  VID GetParent();

  /// @fn SetTransformTRS
  /// @brief
  /// Set the transform.
  /// @param[in] vidBone
  /// The bone identifier.
  /// @param[in] t
  /// The translation.
  /// @param[in] r
  /// The rotation.
  /// @param[in] s
  /// The scale.
  void SetTransformTRS(const BoneVID vidBone, const float t[3],
                       const float r[4], const float s[3]);

  /// @fn UpdateBoneMatrices
  /// @brief
  /// Update the bone matrices.
  void UpdateBoneMatrices();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZSKELETON_H_
