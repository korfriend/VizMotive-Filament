///
/// @file      VzActor.h
/// @brief     The header file for the VzActor class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZACTOR_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZACTOR_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzBaseActor
/// @brief
/// The base class for all actors in the scene.
struct API_EXPORT VzBaseActor : VzSceneComp {
  /// @brief
  /// Constructor for VzBaseActor.
  /// @param[in] vid
  /// The unique identifier for the actor.
  /// @param[in] originFrom
  /// The name of the function that created the actor.
  /// @param[in] typeName
  /// The type name of the actor.
  VzBaseActor(const VID vid, const std::string& originFrom,
              const std::string& typeName,
              const SCENE_COMPONENT_TYPE scenecompType)
      : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

  /// @enum VISIBIE_LAYER
  /// @brief
  /// The visibility layer of the actor.
  enum class VISIBIE_LAYER : uint8_t {
    HIDDEN = 0x0,
    VISIBLE = 0x1,
    GUI = 0x2,
  };

  /// @fn SetVisibleLayer
  /// @brief
  /// Sets the visibility layer of the actor.
  /// @param[in] layer
  /// The visibility layer of the actor.
  void SetVisibleLayer(const VISIBIE_LAYER layer);

  /// @fn GetVisibleLayerMask
  /// @brief
  /// Returns the visibility layer mask of the actor.
  /// @return
  /// The visibility layer mask of the actor.
  uint8_t GetVisibleLayerMask() const;

  /// @fn SetVisibleLayerMask
  /// @brief
  /// Sets the visibility layer mask of the actor.
  /// @param[in] layerBits
  /// The visibility layer bits of the actor.
  /// @param[in] maskBits
  /// The visibility mask bits of the actor.
  void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);

  /// @fn GetPriority
  /// @brief
  /// Returns the priority of the actor.
  /// @return
  /// The priority of the actor.
  uint8_t GetPriority() const;

  /// @fn SetPriority
  /// @brief
  /// Sets the priority of the actor.
  /// @param[in] priority
  /// The priority of the actor.
  void SetPriority(const uint8_t priority);

  /// @fn GetAxisAlignedBoundingBox
  /// @brief
  /// Returns the axis-aligned bounding box of the actor.
  /// @param[out] min
  /// The minimum values of the axis-aligned bounding box.
  /// @param[out] max
  /// The maximum values of the axis-aligned bounding box.
  void GetAxisAlignedBoundingBox(float min[3], float max[3]);
};

/// @struct VzActor
/// @brief
/// The class for actors in the scene.
struct API_EXPORT VzActor : VzBaseActor {
  /// @brief
  /// Constructor for VzActor.
  /// @param[in] vid
  /// The unique identifier for the actor.
  /// @param[in] originFrom
  /// The name of the function that created the actor.
  VzActor(const VID vid, const std::string& originFrom)
      : VzBaseActor(vid, originFrom, "VzActor", SCENE_COMPONENT_TYPE::ACTOR) {}

  /// @fn SetRenderableRes
  /// @brief
  /// Sets the renderable resources of the actor.
  /// @param[in] vidGeo
  /// The unique identifier for the geometry of the actor.
  /// @param[in] vidMIs
  /// The unique identifiers for the material instances of the actor.
  void SetRenderableRes(const VID vidGeo, const std::vector<VID>& vidMIs);

  /// @fn SetMI
  /// @brief
  /// Sets the material instance of the actor.
  /// @param[in] vidMI
  /// The unique identifier for the material instance of the actor.
  /// @param[in] slot
  /// The slot of the material instance of the actor.
  void SetMI(const VID vidMI, const int slot = 0);

  /// @fn SetCastShadows
  /// @brief
  /// Sets whether the actor casts shadows.
  /// @param[in] enabled
  /// Whether the actor casts shadows.
  void SetCastShadows(const bool enabled);

  /// @fn SetReceiveShadows
  /// @brief
  /// Sets whether the actor receives shadows.
  /// @param[in] enabled
  /// Whether the actor receives shadows.
  void SetReceiveShadows(const bool enabled);

  /// @fn SetScreenSpaceContactShadows
  /// @brief
  /// Sets whether the actor receives screen space contact shadows.
  /// @param[in] enabled
  /// Whether the actor receives screen space contact shadows.
  void SetScreenSpaceContactShadows(const bool enabled);

  /// @fn GetMIs
  /// @brief
  /// Returns the unique identifiers for the material instances of the actor.
  /// @return
  /// The unique identifiers for the material instances of the actor.
  std::vector<VID> GetMIs();

  /// @fn GetMI
  /// @brief
  /// Returns the unique identifier for the material instance of the actor.
  /// @param[in] slot
  /// The slot of the material instance of the actor.
  /// @return
  /// The unique identifier for the material instance of the actor.
  VID GetMI(const int slot = 0);

  /// @fn GetMaterial
  /// @brief
  /// Returns the unique identifier for the material of the actor.
  /// @param[in] slot
  /// The slot of the material of the actor.
  /// @return
  /// The unique identifier for the material of the actor.
  VID GetMaterial(const int slot = 0);

  /// @fn GetGeometry
  /// @brief
  /// Returns the unique identifier for the geometry of the actor.
  /// @return
  /// The unique identifier for the geometry of the actor.
  VID GetGeometry();

  /// @fn GetMorphWeights
  /// @brief
  /// Returns the morph weights of the actor.
  /// @param[out] weights
  /// The morph weights of the actor.
  /// @return
  /// The number of morph weights of the actor.
  size_t GetMorphWeights(std::vector<float>& weights);

  /// @fn SetMorphWeights
  /// @brief
  /// Sets the morph weights of the actor.
  /// @param[in] weights
  /// The morph weights of the actor.
  /// @param[in] count
  /// The number of morph weights of the actor.
  void SetMorphWeights(const float* weights, const int count);

  /// @fn GetMorphTargetCount
  /// @brief
  /// Returns the number of morph targets of the actor.
  /// @return
  /// The number of morph targets of the actor.
  int GetMorphTargetCount();
};

/// @struct VzBaseSprite
/// @brief
/// The base class for sprites in the scene.
struct API_EXPORT VzBaseSprite {
 private:
  /// @cond internal
  VzBaseActor* baseActor_;
  /// @endcond

 public:
  /// @brief
  /// Constructor for VzBaseSprite.
  /// @param[in] baseActor
  /// The base actor of the sprite.
  VzBaseSprite(VzBaseActor* baseActor) : baseActor_(baseActor) {}

  /// @fn EnableBillboard
  /// @brief
  /// Enables or disables the billboard effect of the sprite.
  /// @param[in] billboardEnabled
  /// Whether the billboard effect of the sprite is enabled.
  void EnableBillboard(const bool billboardEnabled);

  /// @fn SetRotation
  /// @brief
  /// Sets the rotation of the sprite in degrees.
  /// @param[in] rotDeg
  /// The rotation of the sprite in degrees.
  void SetRotation(const float rotDeg);

  /// @brief
  /// Computes sprite parameters in screen space and converts to world space.
  /// @param[in] x
  /// The x-coordinate of the sprite in screen space.
  /// @param[in] y
  /// The y-coordinate of the sprite in screen space.
  /// @param[in] d
  /// The sprite's depth in screen space (0-1, near to far).
  /// @param[in] w
  /// The sprite's width in screen space.
  /// @param[in] h
  /// The sprite's height in screen space.
  /// @param[in] u
  /// The sprite's anchor point u-coordinate (0-1).
  /// @param[in] v
  /// The sprite's anchor point v-coordinate (0-1).
  /// @param[in] camera
  /// The unique identifier for the camera.
  /// @param[in] renderer
  /// The unique identifier for the renderer.
  /// @param[out] spriteW
  /// The sprite's width in world space.
  /// @param[out] spriteH
  /// The sprite's height in world space.
  /// @param[out] p
  /// The sprite's position in world space.
  static void ComputeScreenSpriteParams(const float x, const float y,
                                        const float d, const float w,
                                        const float h, const float u,
                                        const float v, const VID camera,
                                        const VID renderer, float& spriteW,
                                        float& spriteH, float p[3]);

  /// @fn Raycast
  /// @brief
  /// Performs a raycast on the sprite.
  /// @param[in] origin
  /// The origin of the ray.
  /// @param[in] direction
  /// The direction of the ray.
  /// @param[out] intersects
  /// The hit results of the raycast.
  /// @return
  /// Whether the raycast was successful.
  bool Raycast(const float origin[3], const float direction[3],
               std::vector<HitResult>& intersects);
};

/// @struct VzSpriteActor
/// @brief
/// The class for sprite actors in the scene.
struct API_EXPORT VzSpriteActor : VzBaseActor, VzBaseSprite {
  /// @brief
  /// Constructor for VzSpriteActor.
  /// @param[in] vid
  /// The unique identifier for the actor.
  /// @param[in] originFrom
  /// The name of the function that created the actor.
  VzSpriteActor(const VID vid, const std::string& originFrom)
      : VzBaseActor(vid, originFrom, "VzSpriteActor",
                    SCENE_COMPONENT_TYPE::SPRITE_ACTOR),
        VzBaseSprite(this) {}

  /// @fn GetSpriteWidth
  /// @brief
  /// Returns the width of the sprite.
  /// @return
  /// The width of the sprite.
  float GetSpriteWidth();

  /// @fn GetSpriteHeight
  /// @brief
  /// Returns the height of the sprite.
  /// @return
  /// The height of the sprite.
  float GetSpriteHeight();

  /// @fn GetAnchorU
  /// @brief
  /// Returns the u-coordinate of the anchor point of the sprite.
  /// @return
  /// The u-coordinate of the anchor point of the sprite.
  float GetAnchorU();

  /// @fn GetAnchorV
  /// @brief
  /// Returns the v-coordinate of the anchor point of the sprite.
  /// @return
  /// The v-coordinate of the anchor point of the sprite.
  float GetAnchorV();

  /// @fn SetSpriteWidth
  /// @brief
  /// Sets the width of the sprite.
  /// @param[in] w
  /// The width of the sprite.
  /// @return
  /// The reference to this sprite actor (for method chaining).
  VzSpriteActor& SetSpriteWidth(const float w = 1.f);

  /// @fn SetSpriteHeight
  /// @brief
  /// Sets the height of the sprite.
  /// @param[in] h
  /// The height of the sprite.
  /// @return
  /// The reference to this sprite actor (for method chaining).
  VzSpriteActor& SetSpriteHeight(const float h = 1.f);

  /// @fn SetAnchorU
  /// @brief
  /// Sets the u-coordinate of the anchor point of the sprite.
  /// @param[in] u
  /// The u-coordinate of the anchor point of the sprite.
  /// @return
  /// The reference to this sprite actor (for method chaining).
  VzSpriteActor& SetAnchorU(const float u = 0.5f);

  /// @fn SetAnchorV
  /// @brief
  /// Sets the v-coordinate of the anchor point of the sprite.
  /// @param[in] v
  /// The v-coordinate of the anchor point of the sprite.
  /// @return
  /// The reference to this sprite actor (for method chaining).
  VzSpriteActor& SetAnchorV(const float v = 0.5f);

  /// @fn Build
  /// @brief
  /// Builds the sprite.
  /// @return
  /// Whether the sprite was built successfully.
  bool Build();

  /// @fn GetTexture
  /// @brief
  /// Returns the unique identifier for the texture of the sprite.
  /// @return
  /// The unique identifier for the texture of the sprite.
  VID GetTexture();

  /// @fn SetTexture
  /// @brief
  /// Sets the unique identifier for the texture of the sprite.
  /// @param[in] vidTexture
  /// The unique identifier for the texture of the sprite.
  void SetTexture(const VID vidTexture);
};

/// @enum TEXT_ALIGN
/// @brief
/// The text alignment of the text sprite actor.
enum class TEXT_ALIGN : uint8_t {
  LEFT = 1,
  CENTER = 2,
  RIGHT = 3,
  TOP_LEFT = 4,
  TOP_CENTER = 5,
  TOP_RIGHT = 6,
  MIDDLE_LEFT = 7,
  MIDDLE_CENTER = 8,
  MIDDLE_RIGHT = 9,
  BOTTOM_LEFT = 10,
  BOTTOM_CENTER = 11,
  BOTTOM_RIGHT = 12
};

/// @struct VzTextSpriteActor
/// @brief
/// The class for text sprite actors in the scene.
struct API_EXPORT VzTextSpriteActor : VzBaseActor, VzBaseSprite {
  /// @brief
  /// Constructor for VzTextSpriteActor.
  /// @param[in] vid
  /// The unique identifier for the actor.
  /// @param[in] originFrom
  /// The name of the function that created the actor.
  VzTextSpriteActor(const VID vid, const std::string& originFrom)
      : VzBaseActor(vid, originFrom, "VzTextSpriteActor",
                    SCENE_COMPONENT_TYPE::TEXT_SPRITE_ACTOR),
        VzBaseSprite(this) {}

  /// @fn GetFont
  /// @brief
  /// Returns the unique identifier for the font of the text sprite actor.
  /// @return
  /// The unique identifier for the font of the text sprite actor.
  VID GetFont();

  /// @fn SetFont
  /// @brief
  /// Sets the unique identifier for the font of the text sprite actor.
  /// @param[in] vidFont
  /// The unique identifier for the font of the text sprite actor.
  void SetFont(const VID vidFont);

  /// @fn GetText
  /// @brief
  /// Returns the text of the text sprite actor.
  /// @return
  /// The text of the text sprite actor.
  std::string GetText();

  /// @fn GetTextW
  /// @brief
  /// Returns the wide-character text of the text sprite actor.
  /// @return
  /// The wide-character text of the text sprite actor.
  std::wstring GetTextW();

  /// @fn GetAnchorU
  /// @brief
  /// Returns the u-coordinate of the anchor point of the text sprite actor.
  /// @return
  /// The u-coordinate of the anchor point of the text sprite actor.
  float GetAnchorU();

  /// @fn GetAnchorV
  /// @brief
  /// Returns the v-coordinate of the anchor point of the text sprite actor.
  /// @return
  /// The v-coordinate of the anchor point of the text sprite actor.
  float GetAnchorV();

  /// @fn GetColor
  /// @brief
  /// Returns the color of the text sprite actor.
  /// @param[out] color
  /// The color of the text sprite actor.
  void GetColor(float color[4]);

  /// @fn GetFontHeight
  /// @brief
  /// Returns the font height of the text sprite actor.
  /// @return
  /// The font height of the text sprite actor.
  float GetFontHeight();

  /// @fn GetMaxWidth
  /// @brief
  /// Returns the maximum width of the text sprite actor.
  /// @return
  /// The maximum width of the text sprite actor.
  float GetMaxWidth();

  /// @fn GetTextAlign
  /// @brief
  /// Returns the text alignment of the text sprite actor.
  /// @return
  /// The text alignment of the text sprite actor.
  TEXT_ALIGN GetTextAlign();

  /// @fn SetText
  /// @brief
  /// Sets the text of the text sprite actor.
  /// @param[in] text
  /// The text of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetText(const std::string& text);

  /// @fn SetTextW
  /// @brief
  /// Sets the wide-character text of the text sprite actor.
  /// @param[in] text
  /// The wide-character text of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetTextW(const std::wstring& text);

  /// @fn SetAnchorU
  /// @brief
  /// Sets the u-coordinate of the anchor point of the text sprite actor.
  /// @param[in] anchorU
  /// The u-coordinate of the anchor point of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetAnchorU(const float anchorU);

  /// @fn SetAnchorV
  /// @brief
  /// Sets the v-coordinate of the anchor point of the text sprite actor.
  /// @param[in] anchorV
  /// The v-coordinate of the anchor point of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetAnchorV(const float anchorV);

  /// @fn SetColor
  /// @brief
  /// Sets the color of the text sprite actor.
  /// @param[in] color
  /// The color of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetColor(const float color[4]);

  /// @fn SetFontHeight
  /// @brief
  /// Sets the font height of the text sprite actor.
  /// @param[in] fontHeight
  /// The font height of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetFontHeight(const float fontHeight);

  /// @fn SetMaxWidth
  /// @brief
  /// Sets the maximum width of the text sprite actor.
  /// @param[in] maxWidth
  /// The maximum width of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetMaxWidth(const float maxWidth);

  /// @fn SetTextAlign
  /// @brief
  /// Sets the text alignment of the text sprite actor.
  /// @param[in] textAlign
  /// The text alignment of the text sprite actor.
  /// @return
  /// The reference to this text sprite actor (for method chaining).
  VzTextSpriteActor& SetTextAlign(const TEXT_ALIGN textAlign);

  /// @fn Build
  /// @brief
  /// Builds the text sprite actor.
  void Build();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZACTOR_H_
