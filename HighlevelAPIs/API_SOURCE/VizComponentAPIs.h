///
/// @file      VizComponentAPIs.h
/// @brief     The VizComponentAPIs header file.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_VIZCOMPONENTAPIS_H_
#define HIGHLEVELAPIS_API_SOURCE_VIZCOMPONENTAPIS_H_

#pragma warning(disable : 4003)
#pragma warning(disable : 4251)
#pragma warning(disable : 4819)
#pragma warning(disable : 4146)
#pragma warning(disable : 4068)
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#pragma warning(disable : 4067)

#define _ITERATOR_DEBUG_LEVEL 0

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#define __FP (float*)&
#define VZRESULT int
#define VZ_OK 0
#define VZ_FAIL 1
#define VZ_JOB_WAIT 2
#define VZ_WARNNING 3

#if defined(__clang__)
#define VZ_NONNULL   //_Nonnull
#define VZ_NULLABLE  //_Nullable
#else
#define VZ_NONNULL
#define VZ_NULLABLE
#endif

#include <float.h>

#include <algorithm>
#include <any>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using VID = uint32_t;
inline constexpr VID INVALID_VID = 0;
using TimeStamp = std::chrono::high_resolution_clock::time_point;

constexpr float VZ_PI = 3.141592654f;
constexpr float VZ_2PI = 6.283185307f;
constexpr float VZ_1DIVPI = 0.318309886f;
constexpr float VZ_1DIV2PI = 0.159154943f;
constexpr float VZ_PIDIV2 = 1.570796327f;
constexpr float VZ_PIDIV4 = 0.785398163f;

using uint = uint32_t;

namespace vzm {

/// @struct ParamMap
/// @brief
/// The ParamMap struct.
/// @tparam ID
/// Type used as the identifier for parameters.
template <typename ID>
struct ParamMap {
 private:
  /// @cond internal
  std::string __PM_VERSION = "LIBI_1.4";
  std::unordered_map<ID, std::any> __params;
  /// @endcond

 public:
  /// @fn FindParam
  /// @brief
  /// Find the parameter.
  /// @param[in] param_name
  /// The parameter name.
  /// @return
  /// true if the parameter is found, false otherwise.
  bool FindParam(const ID& param_name) {
    auto it = __params.find(param_name);
    return !(it == __params.end());
  }

  /// @fn GetParamCheck
  /// @brief
  /// Retrieves and checks a parameter of specified type by key.
  /// @tparam SRCV
  /// The type of the parameter to retrieve.
  /// @param[in] key
  /// The key.
  /// @param[out] param
  /// The parameter.
  /// @return
  /// true if the parameter is found, false otherwise.
  template <typename SRCV>
  bool GetParamCheck(const ID& key, SRCV& param) {
    auto it = __params.find(key);
    if (it == __params.end()) return false;
    param = std::any_cast<SRCV&>(it->second);
    return true;
  }

  /// @fn GetParam
  /// @brief
  /// Retrieves a parameter of specified type by key.
  /// @tparam SRCV
  /// The type of the parameter to retrieve.
  /// @param[in] key
  /// The key.
  /// @param[in] init_v
  /// The initial value.
  /// @return
  /// The parameter.
  template <typename SRCV>
  SRCV GetParam(const ID& key, const SRCV& init_v) const {
    auto it = __params.find(key);
    if (it == __params.end()) return init_v;
    return std::any_cast<const SRCV&>(it->second);
  }

  /// @fn GetParamPtr
  /// @brief
  /// Retrieves a parameter pointer of specified type by key.
  /// @tparam SRCV
  /// The type of the parameter to retrieve.
  /// @param[in] key
  /// The key.
  /// @return
  /// The parameter pointer.
  template <typename SRCV>
  SRCV* GetParamPtr(const ID& key) {
    auto it = __params.find(key);
    if (it == __params.end()) return NULL;
    return (SRCV*)&std::any_cast<SRCV&>(it->second);
  }

  /// @fn GetParamCastingCheck
  /// @brief
  /// Retrieves, checks, and casts a parameter of specified types by key.
  /// @tparam SRCV
  /// The source type to cast from.
  /// @tparam DSTV
  /// The destination type to cast to.
  /// @param[in] key
  /// The key.
  /// @param[out] param
  /// The parameter.
  /// @return
  /// true if the parameter is found, false otherwise.
  template <typename SRCV, typename DSTV>
  bool GetParamCastingCheck(const ID& key, DSTV& param) {
    auto it = __params.find(key);
    if (it == __params.end()) return false;
    param = (DSTV)std::any_cast<SRCV&>(it->second);
    return true;
  }

  /// @fn GetParamCasting
  /// @brief
  /// Retrieves and casts a parameter of specified types by key.
  /// @tparam SRCV
  /// The source type to cast from.
  /// @tparam DSTV
  /// The destination type to cast to.
  /// @param[in] key
  /// The key.
  /// @param[in] init_v
  /// The initial value.
  /// @return
  /// The parameter.
  template <typename SRCV, typename DSTV>
  DSTV GetParamCasting(const ID& key, const DSTV& init_v) {
    auto it = __params.find(key);
    if (it == __params.end()) return init_v;
    return (DSTV)std::any_cast<SRCV&>(it->second);
  }

  /// @fn SetParam
  /// @brief
  /// Sets a parameter by key.
  /// @param[in] key
  /// The key.
  /// @param[in] param
  /// The parameter.
  void SetParam(const ID& key, const std::any& param) { __params[key] = param; }

  /// @fn SetString
  /// @brief
  /// Sets a string parameter by key.
  /// @param[in] key
  /// The key.
  /// @param[in] param
  /// The parameter.
  void SetString(const ID& key, const std::string& param) {
    __params[key] = param;
  }

  /// @fn RemoveParam
  /// @brief
  /// Removes a parameter by key.
  /// @param[in] key
  /// The key.
  void RemoveParam(const ID& key) {
    auto it = __params.find(key);
    if (it != __params.end()) {
      __params.erase(it);
    }
  }

  /// @fn RemoveAll
  /// @brief
  /// Removes all parameters.
  void RemoveAll() { __params.clear(); }

  /// @fn Size
  /// @brief
  /// Retrieves the number of parameters.
  /// @return
  /// The number of parameters.
  size_t Size() { return __params.size(); }

  /// @fn GetPMapVersion
  /// @brief
  /// Retrieves the version of the ParamMap.
  /// @return
  /// The version of the ParamMap.
  std::string GetPMapVersion() { return __PM_VERSION; }

  typedef std::unordered_map<ID, std::any> MapType;
  typedef typename MapType::iterator iterator;
  typedef typename MapType::const_iterator const_iterator;
  typedef typename MapType::reference reference;

  /// @fn begin
  /// @brief
  /// Retrieves the beginning iterator.
  /// @return
  /// The beginning iterator.
  iterator begin() { return __params.begin(); }

  /// @fn begin
  /// @brief
  /// Retrieves the beginning const iterator.
  /// @return
  /// The beginning const iterator.
  const_iterator begin() const { return __params.begin(); }

  /// @fn end
  /// @brief
  /// Retrieves the ending iterator.
  /// @return
  /// The ending iterator.
  iterator end() { return __params.end(); }

  /// @fn end
  /// @brief
  /// Retrieves the ending const iterator.
  /// @return
  /// The ending const iterator.
  const_iterator end() const { return __params.end(); }
};

/// @enum SCENE_COMPONENT_TYPE
/// @brief
/// The scene component type.
enum class SCENE_COMPONENT_TYPE {
  SCENEBASE = 0,       //!< empty (only transform and name)
  CAMERA,              //!< camera
  LIGHT_SUN,           //!< sun light
  LIGHT_DIRECTIONAL,   //!< directional light
  LIGHT_POINT,         //!< point light
  LIGHT_FOCUSED_SPOT,  //!< focused spot light
  LIGHT_SPOT,          //!< spot light
  ACTOR,               //!< actor
  SPRITE_ACTOR,        //!< sprite actor
  TEXT_SPRITE_ACTOR    //!< text sprite actor
};

/// @enum RES_COMPONENT_TYPE
/// @brief
/// The resource component type.
enum class RES_COMPONENT_TYPE {
  RESOURCE = 0,      //!< empty
  GEOMATRY,          //!< geometry
  MATERIAL,          //!< material
  MATERIALINSTANCE,  //!< material instance
  TEXTURE,           //!< texture
  FONT               //!< font
};

/// @struct VzBaseComp
/// @brief
/// The base component.
struct API_EXPORT VzBaseComp {
 private:
  /// @cond internal
  VID componentVID_ = INVALID_VID;
  TimeStamp timeStamp_ = {};
  std::string originFrom_;
  std::string type_;
  /// @endcond

 public:
  /// @var attributes
  /// @brief
  /// The attributes.
  ParamMap<std::string> attributes;

  /// @brief
  /// Constructor for VzBaseComp.
  /// @param[in] vid
  /// The unique identifier for the component.
  /// @param[in] originFrom
  /// The name of the function that created the component.
  /// @param[in] typeName
  /// The type name of the component.
  VzBaseComp(const VID vid, const std::string& originFrom,
             const std::string& typeName)
      : componentVID_(vid), originFrom_(originFrom), type_(typeName) {
    UpdateTimeStamp();
  }

  /// @fn GetVID
  /// @brief
  /// Retrieves the entity ID.
  VID GetVID() const { return componentVID_; }

  /// @fn GetType
  /// @brief
  /// Retrieves the type.
  std::string GetType() { return type_; }

  /// @fn GetTimeStamp
  /// @brief
  /// Retrieves the time stamp.
  TimeStamp GetTimeStamp() { return timeStamp_; }

  /// @fn UpdateTimeStamp
  /// @brief
  /// Updates the time stamp.
  void UpdateTimeStamp() {
    timeStamp_ = std::chrono::high_resolution_clock::now();
  }

  /// @fn GetName
  /// @brief
  /// Retrieves the name.
  std::string GetName();

  /// @fn SetName
  /// @brief
  /// Sets the name.
  void SetName(const std::string& name);
};

/// @struct VzSceneComp
/// @brief
/// The scene component.
struct API_EXPORT VzSceneComp : VzBaseComp {
 public:
  /// @enum EULER_ORDER
  /// @brief
  /// The Euler order.
  enum class EULER_ORDER { ZXY, ZYX, XYZ, YXZ, YZX, XZY };

 private:
  /// @cond internal
  SCENE_COMPONENT_TYPE scenecompType_ = SCENE_COMPONENT_TYPE::SCENEBASE;
  float position_[3] = {0.0f, 0.0f, 0.0f};
  float rotation_[3] = {0.0f, 0.0f, 0.0f};
  EULER_ORDER order_ = EULER_ORDER::ZXY;
  float quaternion_[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float scale_[3] = {1.0f, 1.0f, 1.0f};
  bool matrixAutoUpdate_ = false;

  void setQuaternionFromEuler();
  void setEulerFromQuaternion();
  /// @endcond

 public:
  /// @brief
  /// Constructor for VzSceneComp.
  /// @param[in] vid
  /// The unique identifier for the component.
  /// @param[in] originFrom
  /// The name of the function that created the component.
  /// @param[in] typeName
  /// The type name of the component.
  /// @param[in] scenecompType
  /// The scene component type.
  VzSceneComp(const VID vid, const std::string& originFrom,
              const std::string& typeName,
              const SCENE_COMPONENT_TYPE scenecompType)
      : VzBaseComp(vid, originFrom, typeName), scenecompType_(scenecompType) {}

  /// @fn GetSceneCompType
  /// @brief
  /// Retrieves the scene component type.
  /// @return
  /// The scene component type.
  SCENE_COMPONENT_TYPE GetSceneCompType() { return scenecompType_; }

  /// @fn GetWorldPosition
  /// @brief
  /// Retrieves the world position.
  /// @param[out] v
  /// The world position.
  void GetWorldPosition(float v[3]);

  /// @fn GetWorldForward
  /// @brief
  /// Retrieves the world forward vector.
  /// @param[out] v
  /// The world forward vector.
  void GetWorldForward(float v[3]);

  /// @fn GetWorldRight
  /// @brief
  /// Retrieves the world right vector.
  /// @param[out] v
  /// The world right vector.
  void GetWorldRight(float v[3]);

  /// @fn GetWorldUp
  /// @brief
  /// Retrieves the world up vector.
  /// @param[out] v
  /// The world up vector.
  void GetWorldUp(float v[3]);

  /// @fn GetWorldTransform
  /// @brief
  /// Retrieves the world transform.
  /// @param[out] mat
  /// The world transform.
  /// @param[in] rowMajor
  /// The row major flag.
  void GetWorldTransform(float mat[16], const bool rowMajor = false);

  /// @fn GetWorldInvTransform
  /// @brief
  /// Retrieves the world inverse transform.
  /// @param[out] mat
  /// The world inverse transform.
  /// @param[in] rowMajor
  /// The row major flag.
  void GetWorldInvTransform(float mat[16], const bool rowMajor = false);

  /// @fn GetLocalTransform
  /// @brief
  /// Retrieves the local transform.
  /// @param[out] mat
  /// The local transform.
  /// @param[in] rowMajor
  /// The row major flag.
  void GetLocalTransform(float mat[16], const bool rowMajor = false);

  /// @fn GetLocalInvTransform
  /// @brief
  /// Retrieves the local inverse transform.
  /// @param[out] mat
  /// The local inverse transform.
  /// @param[in] rowMajor
  /// The row major flag.
  void GetLocalInvTransform(float mat[16], const bool rowMajor = false);

  /// @fn SetTransform
  /// @brief
  /// Sets the transform.
  /// @param[in] s
  /// The scale.
  /// @param[in] q
  /// The quaternion.
  /// @param[in] t
  /// The translation.
  /// @param[in] additiveTransform
  /// The additive transform flag.
  void SetTransform(const float s[3] = nullptr, const float q[4] = nullptr,
                    const float t[3] = nullptr,
                    const bool additiveTransform = false);

  /// @fn SetMatrix
  /// @brief
  /// Sets the matrix.
  /// @param[in] value
  /// The matrix.
  /// @param[in] additiveTransform
  /// The additive transform flag.
  /// @param[in] rowMajor
  /// The row major flag.
  void SetMatrix(const float value[16], const bool additiveTransform = false,
                 const bool rowMajor = false);

  /// @fn GetParent
  /// @brief
  /// Retrieves the parent entity ID.
  /// @return
  /// The parent entity ID.
  VID GetParent();

  /// @fn GetChildren
  /// @brief
  /// Retrieves the children entity IDs.
  /// @return
  /// The children entity IDs.
  std::vector<VID> GetChildren();

  /// @fn GetScene
  /// @brief
  /// Retrieves the scene entity ID.
  /// @return
  /// The scene entity ID.
  VID GetScene();

  /// @fn GetPosition
  /// @brief
  /// Retrieves the position.
  /// @param[out] position
  /// The position.
  void GetPosition(float position[3]) const;

  /// @fn GetRotation
  /// @brief
  /// Retrieves the rotation.
  /// @param[out] rotation
  /// The rotation.
  /// @param[out] order
  /// The Euler order.
  void GetRotation(float rotation[3], EULER_ORDER* order = nullptr);

  /// @fn GetQuaternion
  /// @brief
  /// Retrieves the quaternion.
  /// @param[out] quaternion
  /// The quaternion.
  void GetQuaternion(float quaternion[4]) const;

  /// @fn GetScale
  /// @brief
  /// Retrieves the scale.
  /// @param[out] scale
  /// The scale.
  void GetScale(float scale[3]) const;

  /// @fn SetPosition
  /// @brief
  /// Sets the position.
  /// @param[in] position
  /// The position.
  void SetPosition(const float position[3]);

  /// @fn SetRotation
  /// @brief
  /// Sets the rotation.
  /// @param[in] rotation
  /// The rotation.
  /// @param[in] order
  /// The Euler order.
  void SetRotation(const float rotation[3],
                   const EULER_ORDER order = EULER_ORDER::XYZ);

  /// @fn SetQuaternion
  /// @brief
  /// Sets the quaternion.
  /// @param[in] quaternion
  /// The quaternion.
  void SetQuaternion(const float quaternion[4]);

  /// @fn SetScale
  /// @brief
  /// Sets the scale.
  /// @param[in] scale
  /// The scale.
  void SetScale(const float scale[3]);

  /// @fn IsMatrixAutoUpdate
  /// @brief
  /// Retrieves the matrix auto-update flag.
  /// @return
  /// true if the matrix is auto-updated, false otherwise.
  bool IsMatrixAutoUpdate() const;

  /// @fn SetMatrixAutoUpdate
  /// @brief
  /// Sets the matrix auto-update flag.
  void SetMatrixAutoUpdate(const bool matrixAutoUpdate);

  /// @fn UpdateMatrix
  /// @brief
  /// Updates the matrix.
  void UpdateMatrix();
};

/// @struct VzResource
/// @brief
/// The resource component.
struct API_EXPORT VzResource : VzBaseComp {
 private:
  /// @cond internal
  RES_COMPONENT_TYPE resType_ = RES_COMPONENT_TYPE::RESOURCE;
  /// @endcond

 public:
  /// @brief
  /// Constructor for VzResource.
  /// @param[in] vid
  /// The unique identifier for the component.
  /// @param[in] originFrom
  /// The name of the function that created the component.
  /// @param[in] typeName
  /// The type name of the component.
  /// @param[in] resType
  /// The resource component type.
  VzResource(const VID vid, const std::string& originFrom,
             const std::string& typeName, const RES_COMPONENT_TYPE resType)
      : VzBaseComp(vid, originFrom, typeName), resType_(resType) {}

  /// @fn GetResType
  /// @brief
  /// Retrieves the resource component type.
  /// @return
  /// The resource component type.
  RES_COMPONENT_TYPE GetResType() { return resType_; }
};

static constexpr size_t MATERIAL_VERSION = 53;

/// @enum UniformType
/// @brief
/// The uniform type.
enum class UniformType : uint8_t {
  BOOL,
  BOOL2,
  BOOL3,
  BOOL4,
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  INT,
  INT2,
  INT3,
  INT4,
  UINT,
  UINT2,
  UINT3,
  UINT4,
  MAT3,  //!< a 3x3 float matrix
  MAT4,  //!< a 4x4 float matrix
  STRUCT
};

/// @enum SamplerType
/// @brief
/// The sampler type.
enum class SamplerType : uint8_t {
  SAMPLER_2D,             //!< 2D texture
  SAMPLER_2D_ARRAY,       //!< 2D array texture
  SAMPLER_CUBEMAP,        //!< Cube map texture
  SAMPLER_EXTERNAL,       //!< External texture
  SAMPLER_3D,             //!< 3D texture
  SAMPLER_CUBEMAP_ARRAY,  //!< Cube map array texture (feature level 2)
};

/// @enum SubpassType
/// @brief
/// The subpass type.
enum class SubpassType : uint8_t { SUBPASS_INPUT };

/// @enum Precision
/// @brief
/// The precision.
enum class Precision : uint8_t { LOW, MEDIUM, HIGH, DEFAULT };

/// @enum RgbType
/// @brief
/// The RGB type.
enum class RgbType : uint8_t {
  sRGB,    //!< the color is defined in Rec.709-sRGB-D65 (sRGB) space
  LINEAR,  //!< the color is defined in Rec.709-Linear-D65 ("linear sRGB") space
};

/// @enum RgbaType
/// @brief
/// The RGBA type.
enum class RgbaType : uint8_t {
  /// the color is defined in Rec.709-sRGB-D65 (sRGB) space and the RGB values
  /// have not been pre-multiplied by the alpha (for instance, a 50% transparent
  /// red is <1,0,0,0.5>)
  sRGB,
  /// the color is defined in Rec.709-Linear-D65 ("linear sRGB") space and the
  /// RGB values have not been pre-multiplied by the alpha (for instance, a 50%
  /// transparent red is <1,0,0,0.5>)
  LINEAR,
  /// the color is defined in Rec.709-sRGB-D65 (sRGB) space and the RGB values
  /// have been pre-multiplied by the alpha (for instance, a 50% transparent red
  /// is <0.5,0,0,0.5>)
  PREMULTIPLIED_sRGB,
  /// the color is defined in Rec.709-Linear-D65 ("linear sRGB") space and the
  /// RGB values have been pre-multiplied by the alpha (for instance, a 50%
  /// transparent red is <0.5,0,0,0.5>)
  PREMULTIPLIED_LINEAR
};

/// @enum SamplerWrapMode
/// @brief
/// The sampler wrap mode.
enum class SamplerWrapMode : uint8_t {
  /// clamp-to-edge. The edge of the texture extends to infinity.
  CLAMP_TO_EDGE,
  /// repeat. The texture infinitely repeats in the wrap direction.
  REPEAT,
  /// mirrored-repeat. The texture infinitely repeats and mirrors in the wrap
  MIRRORED_REPEAT,
};

/// @enum SamplerMinFilter
/// @brief
/// The sampler minification filter.
enum class SamplerMinFilter : uint8_t {
  // don't change the enums values
  /// No filtering. Nearest neighbor is used.
  NEAREST = 0,
  /// Box filtering. Weighted average of 4 neighbors is used.
  LINEAR = 1,
  /// Mip-mapping is activated. But no filtering occurs.
  NEAREST_MIPMAP_NEAREST = 2,
  /// Box filtering within a mip-map level.
  LINEAR_MIPMAP_NEAREST = 3,
  /// Mip-map levels are interpolated, but no other filtering occurs.
  NEAREST_MIPMAP_LINEAR = 4,
  /// Both interpolated Mip-mapping and linear filtering are used.
  LINEAR_MIPMAP_LINEAR = 5
};

/// @enum SamplerMagFilter
/// @brief
/// The sampler magnification filter.
enum class SamplerMagFilter : uint8_t {
  // don't change the enums values
  /// No filtering. Nearest neighbor is used.
  NEAREST = 0,
  /// Box filtering. Weighted average of 4 neighbors is used.
  LINEAR = 1,
};

/// @enum HitResult
/// @brief
/// The hit result.
struct HitResult {
  /// @var distance
  /// @brief
  /// The distance.
  float distance = FLT_MAX;

  /// @var point
  /// @brief
  /// The point.
  float point[3] = {};

  /// @var actor
  /// @brief
  /// The actor.
  VID actor = INVALID_VID;
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_VIZCOMPONENTAPIS_H_
