///
/// @file      VzCamera.h
/// @brief     The header file for the VzCamera class.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZCAMERA_H_
#define HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZCAMERA_H_

#include "../VizComponentAPIs.h"

namespace vzm {

/// @struct VzCamera
/// @brief
/// The VzCamera class is a scene component that represents a camera.
struct API_EXPORT VzCamera : VzSceneComp {
  /// @brief
  /// Constructor for the VzCamera class.
  /// @param[in] vid
  /// The unique identifier for the camera.
  /// @param[in] originFrom
  /// The name of the function that created the camera.
  VzCamera(const VID vid, const std::string& originFrom)
      : VzSceneComp(vid, originFrom, "VzCamera", SCENE_COMPONENT_TYPE::CAMERA) {
  }

  /// @fn SetWorldPose
  /// @brief
  /// Sets the world pose of the camera.
  /// @param[in] pos
  /// The position of the camera.
  /// @param[in] view
  /// The view direction of the camera.
  /// @param[in] up
  /// The up direction of the camera.
  void SetWorldPose(const float pos[3], const float view[3], const float up[3]);

  /// @fn SetPerspectiveProjection
  /// @brief
  /// Sets the perspective projection of the camera.
  /// @param[in] zNearP
  /// The distance to the near plane.
  /// @param[in] zFarP
  /// The distance to the far plane.
  /// @param[in] fovInDegree
  /// The field of view in degrees.
  /// @param[in] aspectRatio
  /// The aspect ratio.
  /// @param[in] isVertical
  /// The flag to indicate whether the field of view is vertical.
  void SetPerspectiveProjection(const float zNearP, const float zFarP,
                                const float fovInDegree,
                                const float aspectRatio,
                                const bool isVertical = true);

  /// @fn GetWorldPose
  /// @brief
  /// Gets the world pose of the camera.
  /// @param[out] pos
  /// The position of the camera.
  /// @param[out] view
  /// The view direction of the camera.
  /// @param[out] up
  /// The up direction of the camera.
  void GetWorldPose(float pos[3], float view[3], float up[3]);

  /// @fn GetPerspectiveProjection
  /// @brief
  /// Gets the perspective projection of the camera.
  /// @param[out] zNearP
  /// The distance to the near plane.
  /// @param[out] zFarP
  /// The distance to the far plane.
  /// @param[out] fovInDegree
  /// The field of view in degrees.
  /// @param[out] aspectRatio
  /// The aspect ratio.
  /// @param[in] isVertical
  /// The flag to indicate whether the field of view is vertical.
  void GetPerspectiveProjection(float* zNearP, float* zFarP, float* fovInDegree,
                                float* aspectRatio, bool isVertical = true);

  /// @fn SetCameraCubeVisibleLayerMask
  /// @brief
  /// Sets the visible layer mask of the camera cube.
  /// @param[in] layerBits
  /// The layer bits.
  /// @param[in] maskBits
  /// The mask bits.
  void SetCameraCubeVisibleLayerMask(const uint8_t layerBits = 0x3,
                                     const uint8_t maskBits = 0x2);

  /// @fn SetLensProjection
  /// @brief
  /// Sets the lens projection of the camera.
  /// @param[in] focalLengthInMillimeters
  /// The focal length in millimeters.
  /// @param[in] aspect
  /// The aspect ratio.
  /// @param[in] near
  /// The distance to the near plane.
  /// @param[in] far
  /// The distance to the far plane
  void SetLensProjection(float focalLengthInMillimeters, float aspect,
                         float near, float far);

  /// @fn GetNear
  /// @brief
  /// Gets the distance to the near plane.
  /// @return
  /// The distance to the near plane.
  float GetNear();

  /// @fn GetCullingFar
  /// @brief
  /// Gets the distance to the far plane.
  /// @return
  /// The distance to the far plane.
  float GetCullingFar();

  /// @fn SetExposure
  /// @brief
  /// Sets the exposure of the camera.
  /// @param[in] aperture
  /// The aperture.
  /// @param[in] shutterSpeed
  /// The shutter speed.
  /// @param[in] sensitivity
  /// The sensitivity.
  void SetExposure(float aperture, float shutterSpeed, float sensitivity);

  /// @fn GetAperture
  /// @brief
  /// Gets the aperture of the camera.
  /// @return
  /// The aperture of the camera.
  float GetAperture();

  /// @fn GetShutterSpeed
  /// @brief
  /// Gets the shutter speed of the camera.
  /// @return
  /// The shutter speed of the camera.
  float GetShutterSpeed();

  /// @fn GetSensitivity
  /// @brief
  /// Gets the sensitivity of the camera.
  /// @return
  /// The sensitivity of the camera.
  float GetSensitivity();

  /// @fn GetFocalLength
  /// @brief
  /// Gets the focal length of the camera.
  /// @return
  /// The focal length of the camera.
  float GetFocalLength();

  /// @fn SetFocusDistance
  /// @brief
  /// Sets the focus distance of the camera.
  /// @param[in] distance
  /// The focus distance.
  void SetFocusDistance(float distance);

  /// @fn GetFocusDistance
  /// @brief
  /// Gets the focus distance of the camera.
  /// @return
  /// The focus distance of the camera.
  float GetFocusDistance();

  /// @struct Controller
  /// @brief
  /// The Controller struct is a camera manipulator.
  struct API_EXPORT Controller {
   private:
    /// @cond internal
    VID vidCam_ = INVALID_VID;
    /// @endcond

   public:
    /// @brief
    /// Constructor for the Controller struct.
    /// @param[in] vidCam
    /// The unique identifier for the camera.
    Controller(VID vidCam) { vidCam_ = vidCam; }

    /// @fn GetCameraVID
    /// @brief
    /// Gets the unique identifier for the camera.
    /// @return
    /// The unique identifier for the camera.
    VID GetCameraVID() { return vidCam_; }

    /// @var targetPosition
    /// @brief
    /// The world-space position of interest.
    float targetPosition[3] = {0, 0, 0};

    /// @var upVector
    /// @brief
    /// The orientation for the home position.
    float upVector[3] = {0, 1.f, 0};

    /// @var zoomSpeed
    /// @brief
    /// Multiplied with scroll delta
    float zoomSpeed = 0.01f;

    /// @var orbitHomePosition
    /// @brief
    /// Initial eye position in world space
    float orbitHomePosition[3] = {0, 0, 1.f};

    /// @var orbitSpeed
    /// @brief
    /// Multiplied with viewport delta
    float orbitSpeed[2] = {0.01f, 0.01f};

    /// @var minPolarAngle
    /// @brief
    /// Minimum polar angle (radians) for orbit mode.
    float minPolarAngle = 0.f;

    /// @var maxPolarAngle
    /// @brief
    /// Maximum polar angle (radians) for orbit mode.
    float maxPolarAngle = VZ_PI;

    /// @var minAzimuthAngle
    /// @brief
    /// Minimum azimuth angle (radians) for orbit mode.
    float minAzimuthAngle = -INFINITY;

    /// @var maxAzimuthAngle
    /// @brief
    /// Maximum azimuth angle (radians) for orbit mode.
    float maxAzimuthAngle = INFINITY;

    /// @var isVerticalFov
    /// @brief
    /// The axis that's held constant when viewport changes for map mode.
    bool isVerticalFov = true;

    /// @var fovDegrees
    /// @brief
    /// The full FOV (not the half-angle) for map mode.
    float fovDegrees = 90.f;

    /// @var farPlane
    /// @brief
    /// The distance to the far plane for map mode.
    float farPlane = 1000.f;

    /// @var mapExtent
    /// @brief
    /// The ground size for computing home position.
    float mapExtent[2] = {10.f, 10.f};

    /// @var mapMinDistance
    /// @brief
    /// Constrains the zoom-in level.
    float mapMinDistance = 0.01f;

    /// @var flightStartPosition
    /// @brief
    /// Initial eye position in world space.
    float flightStartPosition[3];

    /// @var flightStartPitch
    /// @brief
    /// Initial pitch angle in radians.
    float flightStartPitch = 0;

    /// @var flightStartYaw
    /// @brief
    /// Initial yaw angle in radians.
    float flightStartYaw = 0;

    /// @var flightMaxSpeed
    /// @brief
    /// The maximum camera speed in world units per second.
    float flightMaxSpeed = 10.f;

    /// @var flightSpeedSteps
    /// @brief
    /// The number of speed steps adjustable with scroll wheel.
    int flightSpeedSteps = 80;

    /// @var flightSpeedSteps
    /// @brief
    /// Multiplied with viewport delta.
    float flightPanSpeed[2] = {0.01f, 0.01f};

    /// @var flightMoveDamping
    /// @brief
    /// Applies a deceleration to camera movement, 0 (no damping).
    float flightMoveDamping = 15.f;

    /// @var groundPlane
    /// @brief
    /// Plane equation used as a raycast fallback.
    float groundPlane[4] = {0, 1.f, 0, 0};

    /// @var raycastCallback
    /// @brief
    /// Raycast function for accurate grab-and-pan.
    void* raycastCallback = nullptr;

    /// @var raycastUserdata
    /// @brief
    /// User data for the raycast function.
    void* raycastUserdata = nullptr;

    /// @var panning
    /// @brief
    /// Sets whether panning is enabled.
    bool panning = true;

    /// @enum Mode
    /// @brief
    /// The Mode enum class represents the camera manipulator mode.
    enum class Mode { ORBIT, MAP, FREE_FLIGHT };

    /// @var mode
    /// @brief
    /// The camera manipulator mode.
    Mode mode = Mode::ORBIT;

    /// @fn UpdateControllerSettings
    /// @brief
    /// Call this to apply the current configuration.
    void UpdateControllerSettings();

    /// @enum Key
    /// @brief
    /// The Key enum class represents the camera manipulator key.
    enum class Key { FORWARD, LEFT, BACKWARD, RIGHT, UP, DOWN, COUNT };

    /// @fn KeyDown
    /// @brief
    /// Call this when a key is pressed.
    void KeyDown(const Key key);

    /// @fn KeyUp
    /// @brief
    /// Call this when a key is released.
    void KeyUp(const Key key);

    /// @fn Scroll
    /// @brief
    /// Call this when the mouse wheel is scrolled.
    /// @param[in] x
    /// The x-coordinate.
    /// @param[in] y
    /// The y-coordinate.
    /// @param[in] scrollDelta
    void Scroll(const int x, const int y, const float scrollDelta);

    /// @fn GrabBegin
    /// @brief
    /// Call this when a grab begins.
    /// @param[in] x
    /// The x-coordinate.
    /// @param[in] y
    /// The y-coordinate.
    /// @param[in] strafe
    /// The flag to indicate whether strafing is enabled.
    void GrabBegin(const int x, const int y, const bool strafe);

    /// @fn GrabDrag
    /// @brief
    /// Call this when a grab is dragged.
    /// @param[in] x
    /// The x-coordinate.
    /// @param[in] y
    /// The y-coordinate.
    void GrabDrag(const int x, const int y);

    /// @fn GrabEnd
    /// @brief
    /// Call this when a grab ends.
    void GrabEnd();

    /// @fn SetViewport
    /// @brief
    /// Sets the viewport.
    void SetViewport(const int w, const int h);

    /// @fn UpdateCamera
    /// @brief
    /// This is for final sync to the camera.
    /// @param[in] deltaTime
    /// The time since the last update.
    /// @return
    /// The updated camera.
    void UpdateCamera(const float deltaTime);

    /// @fn SetOrbitPhi
    /// @brief
    /// Sets the orbit phi.
    /// @param[in] phi
    /// The orbit phi.
    void SetOrbitPhi(const float phi);

    /// @fn GetOrbitPhi
    /// @brief
    /// Gets the orbit phi.
    /// @return
    /// The orbit phi.
    float GetOrbitPhi();

    /// @fn SetOrbitTheta
    /// @brief
    /// Sets the orbit theta.
    /// @param[in] theta
    /// The orbit theta.
    void SetOrbitTheta(const float theta);

    /// @fn GetOrbitTheta
    /// @brief
    /// Gets the orbit theta.
    /// @return
    /// The orbit theta.
    float GetOrbitTheta();

    /// @fn SetOrbitDistance
    /// @brief
    /// Sets the orbit distance.
    /// @param[in] distance
    /// The orbit distance.
    /// @return
    /// The orbit distance.
    void SetOrbitDistance(const float distance);

    /// @fn GetOrbitDistance
    /// @brief
    /// Gets the orbit distance.
    /// @return
    /// The orbit distance.
    float GetOrbitDistance();

    /// @fn SetFlightPitch
    /// @brief
    /// Sets the flight pitch.
    /// @param[in] pitch
    /// The flight pitch.
    void SetFlightPitch(const float pitch);

    /// @fn GetFlightPitch
    /// @brief
    /// Gets the flight pitch.
    /// @return
    /// The flight pitch.
    float GetFlightPitch();

    /// @fn SetFlightYaw
    /// @brief
    /// Sets the flight yaw.
    /// @param[in] yaw
    /// The flight yaw.
    void SetFlightYaw(const float yaw);

    /// @fn GetFlightYaw
    /// @brief
    /// Gets the flight yaw.
    float GetFlightYaw();
  };

  /// @fn GetController
  /// @brief
  /// Gets the camera manipulator.
  /// @return
  /// The camera manipulator.
  Controller* GetController();

  /// @fn SetControllerEnabled
  /// @brief
  /// Sets whether the camera manipulator is enabled.
  /// @param[in] enabled
  /// The flag to indicate whether the camera manipulator is enabled.
  void SetControllerEnabled(bool enabled);

  /// @fn IsControllerEnabled
  /// @brief
  /// Gets whether the camera manipulator is enabled.
  /// @return
  /// The flag to indicate whether the camera manipulator is enabled.
  bool IsControllerEnabled();
};

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_COMPONENTS_VZCAMERA_H_
