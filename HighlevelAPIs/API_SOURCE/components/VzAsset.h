///
/// @file      VzAsset.h
/// @brief     The header file for the VzAsset class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZASSET_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZASSET_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzAsset
/// @brief
/// Owns a hierarchy of entities loaded from a glTF asset
struct API_EXPORT VzAsset : VzBaseComp {
  /// @brief
  /// Constructor for VzAsset.
  /// @param[in] vid
  /// The unique identifier for the VzAsset.
  /// @param[in] originFrom
  /// The origin of the VzAsset.
  VzAsset(const VID vid, const std::string& originFrom)
      : VzBaseComp(vid, originFrom, "VzAsset") {}

  /// @fn GetGLTFRoots
  /// @brief
  /// Returns the root entities of the glTF asset.
  /// @return
  /// A vector of unique identifiers for the root entities.
  std::vector<VID> GetGLTFRoots();

  /// @fn GetSkeletons
  /// @brief
  /// Returns the skeletons of the glTF asset.
  /// @return
  /// A vector of unique identifiers for the skeletons.
  std::vector<VID> GetSkeletons();

  /// @fn GetVariantsCount
  /// @brief
  /// Returns the number of material variants in the glTF asset.
  /// @return
  /// The number of material variants.
  size_t GetVariantsCount();

  /// @fn GetVariantName
  /// @brief
  /// Returns the name of the material variant at the specified index.
  /// @param[in] variantIndex
  /// The index of the material variant.
  /// @return
  /// The name of the material variant.
  std::string GetVariantName(int variantIndex);

  /// @fn ApplyMaterialvariant
  /// @brief
  /// Applies the material variant at the specified index.
  /// @param[in] variantIndex
  /// The index of the material variant.
  void ApplyMaterialvariant(int variantIndex);

  /// @fn GetComponents
  /// @brief
  /// Returns the components of the VzAsset.
  /// @param[out] components
  /// A vector of unique identifiers for the components.
  size_t GetComponents(std::vector<VID>& components);

  /// @fn IsAssetOwned
  /// @brief
  /// Checks if the component is owned by the VzAsset.
  /// @param[in] vid
  /// The unique identifier for the component.
  /// @param[out] typeName
  /// The type name of the component.
  /// @return
  /// True if the component is owned by the VzAsset, false otherwise.
  bool IsAssetOwned(const VID vid, std::string* VZ_NULLABLE typeName = nullptr);

  /// @fn IsAssetOwnedComp
  /// @brief
  /// Checks if the component is owned by the VzAsset.
  /// @param[in] comp
  /// The component.
  /// @param[out] typeName
  /// The type name of the component.
  /// @return
  /// True if the component is owned by the VzAsset, false otherwise.
  bool IsAssetOwnedComp(const VzBaseComp* comp,
                        std::string* VZ_NULLABLE typeName = nullptr) {
    return IsAssetOwned(comp->GetVID(), typeName);
  }

  /// @struct Animator
  /// @brief
  /// Manages the animations of the VzAsset.
  struct API_EXPORT Animator {
   public:
    /// @enum PlayMode
    /// @brief
    /// The play modes for the animations.
    enum class PlayMode {
      INIT_POSE,
      PLAY,
      PAUSE,
    };

   private:
    /// @cond internal
    VID vidAsset_ = INVALID_VID;
    std::set<size_t> activatedAnimations_ = {};
    size_t animationIndex_ = 0;
    int crossFadeAnimationIndex_ = -1;
    int crossFadePrevAnimationIndex_ = -1;
    double crossFadeDurationSec_ = 1.0;
    TimeStamp timer_ = {};
    double elapsedTimeSec_ = 0.0;
    double prevElapsedTimeSec_ = 0.0;
    double fixedUpdateTime_ = 1. / 60.;
    std::set<VID> associatedScenes_;
    PlayMode playMode_ = PlayMode::INIT_POSE;
    bool resetAnimation_ = true;
    /// @endcond

   public:
    /// @brief
    /// Constructor for Animator.
    /// @param[in] vidAsset
    /// The unique identifier for the VzAsset.
    Animator(VID vidAsset) { vidAsset_ = vidAsset; }

    /// @fn AddPlayScene
    /// @brief
    /// Adds a scene to the list of associated scenes.
    /// @param[in] vidScene
    /// The unique identifier for the scene.
    /// @return
    /// The number of associated scenes.
    size_t AddPlayScene(const VID vidScene) {
      associatedScenes_.insert(vidScene);
      return associatedScenes_.size();
    }

    /// @fn RemovePlayScene
    /// @brief
    /// Removes a scene from the list of associated scenes.
    /// @param[in] vidScene
    /// The unique identifier for the scene.
    /// @return
    /// The number of associated scenes.
    size_t RemovePlayScene(const VID vidScene) {
      associatedScenes_.erase(vidScene);
      return associatedScenes_.size();
    }

    /// @fn IsPlayScene
    /// @brief
    /// Checks if the scene is in the list of associated scenes.
    /// @param[in] vidScene
    /// The unique identifier for the scene.
    /// @return
    /// True if the scene is in the list of associated scenes, false otherwise.
    bool IsPlayScene(const VID vidScene) {
      return associatedScenes_.contains(vidScene);
    }

    /// @fn GetAssetVID
    /// @brief
    /// Returns the unique identifier for the VzAsset.
    /// @return
    /// The unique identifier for the VzAsset.
    VID GetAssetVID() { return vidAsset_; }

    /// @fn GetAnimationCount
    /// @brief
    /// Returns the number of animations.
    /// @return
    /// The number of animations.
    size_t GetAnimationCount();

    /// @fn GetAnimationLabel
    /// @brief
    /// Returns the label of the animation at the specified index.
    /// @param[in] index
    /// The index of the animation.
    /// @return
    /// The label of the animation.
    std::string GetAnimationLabel(const int index);

    /// @fn GetAnimationLabels
    /// @brief
    /// Returns the labels of the animations.
    /// @return
    /// A vector of labels for the animations.
    std::vector<std::string> GetAnimationLabels();

    /// @fn ApplyAnimationTimeAt
    /// @brief
    /// Applies the animation time at the specified index.
    /// @param[in] index
    /// The index of the animation.
    /// @param[in] elapsedTime
    /// The elapsed time.
    void ApplyAnimationTimeAt(const size_t index, const float elapsedTime);

    /// @fn UpdateBoneMatrices
    /// @brief
    /// Updates the bone matrices.
    void UpdateBoneMatrices();

    /// @fn ActivateAnimation
    /// @brief
    /// Activates the animation at the specified index.
    /// @param[in] index
    /// The index of the animation.
    /// @return
    /// The label of the animation.
    std::string ActivateAnimation(const size_t index) {
      activatedAnimations_.insert(index);
      return GetAnimationLabel(index);
    }

    /// @fn ActivateAnimationByLabel
    /// @brief
    /// Activates the animation with the specified label.
    /// @param[in] label
    /// The label of the animation.
    /// @return
    /// The index of the animation.
    int ActivateAnimationByLabel(const std::string& label);

    /// @fn DeactivateAnimation
    /// @brief
    /// Deactivates the animation at the specified index.
    /// @param[in] index
    /// The index of the animation.
    /// @return
    /// The label of the animation.
    std::string DeactivateAnimation(const size_t index) {
      activatedAnimations_.erase(index);
      return GetAnimationLabel(index);
    }

    /// @fn DeactivateAnimationByLabel
    /// @brief
    /// Deactivates the animation with the specified label.
    /// @param[in] label
    /// The label of the animation.
    /// @return
    /// The index of the animation.
    int DeactivateAnimationByLabel(const std::string& label);

    /// @fn DeactivateAll
    /// @brief
    /// Deactivates all animations.
    /// @return
    /// The number of deactivated animations.
    int DeactivateAll();

    /// @fn SetCrossFadeDuration
    /// @brief
    /// Sets the crossfade duration between the animations.
    /// @param[in] crossFadeAnimationIndex
    /// The index of the animation to crossfade to.
    /// @param[in] crossFadePrevAnimationIndex
    /// The index of the previous animation.
    /// @param[in] timeSec
    /// The duration of the crossfade in seconds.
    void SetCrossFadeDuration(const int crossFadeAnimationIndex,
                              const int crossFadePrevAnimationIndex,
                              const double timeSec = 1.) {
      crossFadeAnimationIndex_ = crossFadeAnimationIndex;
      crossFadePrevAnimationIndex_ = crossFadePrevAnimationIndex;
      crossFadeDurationSec_ = timeSec;
    }

    /// @fn GetAnimationPlayTime
    /// @brief
    /// Returns the play time of the animation at the specified index.
    /// @param[in] index
    /// The index of the animation.
    /// @return
    /// The play time of the animation.
    float GetAnimationPlayTime(const size_t index);

    /// @fn GetAnimationPlayTimeByLabel
    /// @brief
    /// Returns the play time of the animation with the specified label.
    /// @param[in] label
    /// The label of the animation.
    /// @return
    /// The play time of the animation.
    float GetAnimationPlayTimeByLabel(const std::string& label);

    /// @fn MovePlayTime
    /// @brief
    /// Moves the play time.
    /// @param[in] elsapsedTimeSec
    /// The elapsed time in seconds.
    void MovePlayTime(const double elsapsedTimeSec) {
      elapsedTimeSec_ = elsapsedTimeSec;
    }

    /// @fn GetPlayTime
    /// @brief
    /// Returns the play time.
    /// @return
    /// The play time.
    double GetPlayTime() { return elapsedTimeSec_; }

    /// @fn SetPlayMode
    /// @brief
    /// Sets the play mode.
    /// @param[in] playMode
    /// The play mode.
    void SetPlayMode(const PlayMode playMode) {
      playMode_ = playMode;
      resetAnimation_ = playMode == PlayMode::INIT_POSE;
    }

    /// @fn GetPlayMode
    /// @brief
    /// Returns the play mode.
    /// @return
    /// The play mode.
    PlayMode GetPlayMode() { return playMode_; }

    /// @fn Reset
    /// @brief
    /// Resets the animation.
    void Reset() { resetAnimation_ = true; }

    /// @fn UpdateAnimation
    /// @brief
    /// Updates the animation. This function is called in the renderer.
    void UpdateAnimation();
  };

  /// @fn GetAnimator
  /// @brief
  /// Returns the animator of the VzAsset.
  Animator* GetAnimator();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZASSET_H_
