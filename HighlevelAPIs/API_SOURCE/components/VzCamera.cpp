#include "VzCamera.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

extern Engine* gEngine;
extern VzEngineApp* gEngineApp;

namespace vzm
{
    using CameraManipulator = filament::camutils::Manipulator<float>;

    // Pose parameters are defined in WS (not local space)
    void VzCamera::SetWorldPose(const float pos[3], const float view[3], const float up[3])
    {
        COMP_TRANSFORM(tc, ett, ins, );

        // up vector correction
        double3 _eye = *(float3*)pos;
        double3 _view = normalize((double3) * (float3*)view);
        double3 _up = *(float3*)up;
        double3 _right = cross(_view, _up);
        _up = normalize(cross(_right, _view));

        // note the pose info is defined in WS
        //mat4f ws2cs = mat4f::lookTo(_view, _eye, _up);
        //mat4f cs2ws = inverse(ws2cs);
        Camera* camera = gEngine->getCameraComponent(ett);
        camera->lookAt(_eye, _eye + _view, _up);
        mat4 ws2cs_d = camera->getViewMatrix();
        mat4 cs2ws_d = inverse(ws2cs_d);

        Entity ett_parent = tc.getParent(ins);
        mat4 parent2ws_d = mat4();
        while (!ett_parent.isNull())
        {
            auto ins_parent = tc.getInstance(ett_parent);
            parent2ws_d = mat4(tc.getTransform(ins_parent)) * parent2ws_d;
            ett_parent = tc.getParent(ins_parent);
        }

        mat4f local = mat4f(inverse(parent2ws_d) * cs2ws_d);
        SetMatrix((float*)&local[0][0], false, false);
    }
    void VzCamera::SetPerspectiveProjection(const float zNearP, const float zFarP, const float fovInDegree, const float aspectRatio, const bool isVertical)
    {
        COMP_CAMERA(camera, ett, );
        // aspectRatio is W / H
        camera->setProjection(fovInDegree, aspectRatio, zNearP, zFarP,
            isVertical ? Camera::Fov::VERTICAL : Camera::Fov::HORIZONTAL);
        camera->setScaling(1.0f);
        UpdateTimeStamp();
    }

    void VzCamera::SetCameraCubeVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits)
    {
        VzCameraRes* cam_res = gEngineApp->GetCameraRes(GetVID());
        if (cam_res == nullptr) return;

        VzCube* camera_cube = cam_res->GetCameraCube();
        auto& rcm = gEngine->getRenderableManager();
        rcm.setLayerMask(rcm.getInstance(camera_cube->getSolidRenderable()), layerBits, maskBits);
        rcm.setLayerMask(rcm.getInstance(camera_cube->getWireFrameRenderable()), layerBits, maskBits);

        cubeToScene(camera_cube->getSolidRenderable().getId(), GetVID());
        cubeToScene(camera_cube->getWireFrameRenderable().getId(), GetVID());
        UpdateTimeStamp();
    }

    void VzCamera::GetWorldPose(float pos[3], float view[3], float up[3])
    {
        COMP_CAMERA(camera, ett, );
        double3 p = camera->getPosition();
        double3 v = camera->getForwardVector();
        double3 u = camera->getUpVector();
        if (pos) *(float3*)pos = float3(p);
        if (view) *(float3*)view = float3(v);
        if (up) *(float3*)up = float3(u);
    }
    void VzCamera::GetPerspectiveProjection(float* zNearP, float* zFarP, float* fovInDegree, float* aspectRatio, bool isVertical)
    {
        COMP_CAMERA(camera, ett, );
        if (zNearP) *zNearP = (float)camera->getNear();
        if (zFarP) *zFarP = (float)camera->getCullingFar();
        if (fovInDegree) *fovInDegree = (float)camera->getFieldOfViewInDegrees(isVertical ? Camera::Fov::VERTICAL : Camera::Fov::HORIZONTAL);
        if (aspectRatio)
        {
            mat4 mat_proj = camera->getProjectionMatrix();
            *aspectRatio = (float)(mat_proj[1][1] / mat_proj[0][0]);
        }
    }
    void VzCamera::SetLensProjection(float focalLengthInMillimeters, float aspect, float near, float far)
    {
        COMP_CAMERA(camera, ett, );
        camera->setLensProjection(focalLengthInMillimeters, aspect, near, far);
        camera->setScaling(1.0f);
        UpdateTimeStamp();
    }
    float VzCamera::GetNear()
    {
        COMP_CAMERA(camera, ett, 0.0f);
        return (float) camera->getNear();
    }
    float VzCamera::GetCullingFar()
    {
        COMP_CAMERA(camera, ett, 0.0f);
        return (float) camera->getCullingFar();
    }
    void VzCamera::SetExposure(float aperture, float shutterSpeed, float sensitivity)
    {
        COMP_CAMERA(camera, ett, );
        camera->setExposure(aperture, shutterSpeed, sensitivity);
        UpdateTimeStamp();
    }
    float VzCamera::GetAperture()
    {
        COMP_CAMERA(camera, ett, 16.0f);
        return (float) camera->getAperture();
    }
    float VzCamera::GetShutterSpeed()
    {
        COMP_CAMERA(camera, ett, 1.0f / 125.0f);
        return (float) camera->getShutterSpeed();
    }
    float VzCamera::GetSensitivity()
    {
        COMP_CAMERA(camera, ett, 100.0f);
        return (float) camera->getSensitivity();
    }
    float VzCamera::GetFocalLength()
    {
        COMP_CAMERA(camera, ett, 0);
        return (float) camera->getFocalLength() * 1000.0f;
    }
    void VzCamera::SetFocusDistance(float distance)
    {
        COMP_CAMERA(camera, ett, );
        camera->setFocusDistance(distance);
        UpdateTimeStamp();
    }
    float VzCamera::GetFocusDistance()
    {
        COMP_CAMERA(camera, ett, 0);
        return (float) camera->getFocusDistance();
    }

    VzCamera::Controller* VzCamera::GetController()
    {
        VzCameraRes* cam_res = gEngineApp->GetCameraRes(GetVID());
        if (cam_res == nullptr) return nullptr;
        CameraManipulator* cm = cam_res->GetCameraManipulator();
        Controller* cc = cam_res->GetCameraController();
        if (cm == nullptr)
        {
            Controller controller(GetVID());
            cam_res->NewCameraManipulator(controller);
            cc = cam_res->GetCameraController();
        }
        return cc;
    }
    void VzCamera::SetControllerEnabled(bool enabled) {
        VzCameraRes* cam_res = gEngineApp->GetCameraRes(GetVID());
        if (cam_res == nullptr) return;
        cam_res->cameraControllerEnabled = enabled;
    }
    bool VzCamera::IsControllerEnabled() {
        VzCameraRes* cam_res = gEngineApp->GetCameraRes(GetVID());
        if (cam_res == nullptr) return false;
        return cam_res->cameraControllerEnabled;
    }
#define GET_CM(CAMRES, CM, FAILRET) VzCameraRes* CAMRES = gEngineApp->GetCameraRes(GetCameraVID()); if (CAMRES == nullptr) return FAILRET;  CameraManipulator* CM = CAMRES->GetCameraManipulator();
#define GET_CM_WARN(CAMRES, CM, FAILRET) GET_CM(CAMRES, CM, FAILRET) if (CM == nullptr) { backlog::post("camera manipulator is not set!", backlog::LogLevel::Warning); return FAILRET; }
    struct Bookmark
    {
        struct MapParams
        {
            float extent;
            float center[2];
        };
        struct OrbitParams
        {
            float phi;
            float theta;
            float distance;
            float pivot[3];
        };
        struct FlightParams
        {
            float pitch;
            float yaw;
            float position[3];
        };
        vzm::VzCamera::Controller::Mode mode;
        MapParams map;
        OrbitParams orbit;
        FlightParams flight;
    };
    void VzCamera::Controller::UpdateControllerSettings()
    {
        GET_CM(cam_res, cm, );
        cam_res->NewCameraManipulator(*this);
    }
    void VzCamera::Controller::KeyDown(const Key key)
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->keyDown((CameraManipulator::Key)key);
    }
    void VzCamera::Controller::KeyUp(const Key key)
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->keyUp((CameraManipulator::Key)key);
    }
    void VzCamera::Controller::Scroll(const int x, const int y, const float scrollDelta)
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->scroll(x, cam_res->height - y, scrollDelta);
    }
    void VzCamera::Controller::GrabBegin(const int x, const int y, const bool strafe)
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->grabBegin(x, cam_res->height - y, strafe);
    }
    void VzCamera::Controller::GrabDrag(const int x, const int y)
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->grabUpdate(x, cam_res->height - y);
        if (mode == Mode::ORBIT)
        {
            camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
            Bookmark bookmark = *(Bookmark*)&camutilsBookmark;
            float& phi = bookmark.orbit.phi;
            float& theta = bookmark.orbit.theta;
            float _min = minAzimuthAngle;
            float _max = maxAzimuthAngle;
            if (!isinf(_min) && !isinf(_max))
            {
                if (_min < -VZ_PI) _min += VZ_2PI; else if (_min > VZ_PI) _min -= VZ_2PI;
                if (_max < -VZ_PI) _max += VZ_2PI; else if (_max > VZ_PI) _max -= VZ_2PI;
                if (_min <= _max)
                {
                    theta = clamp(theta, _min, _max);
                }
                else
                {
                    theta = (theta > (_min + _max) / 2) ? max(_min, theta)
                                                        : min(_max, theta);
                }
            }
            const float EPS = 0.000001f;
            phi = clamp(phi, minPolarAngle, maxPolarAngle);
            cm->jumpToBookmark(*(camutils::Bookmark<float>*)&bookmark);
        }
    }
    void VzCamera::Controller::GrabEnd()
    {
        GET_CM_WARN(cam_res, cm, );
        if (!cam_res->cameraControllerEnabled) return;
        cm->grabEnd();
    }
    void VzCamera::Controller::SetViewport(const int w, const int h)
    {
        GET_CM_WARN(cam_res, cm, );
        cam_res->width = w;
        cam_res->height = h;
        cm->setViewport(w, h);
    }
    void VzCamera::Controller::UpdateCamera(const float deltaTime)
    {
        GET_CM_WARN(cam_res, cm, );
        cam_res->UpdateCameraWithCM(deltaTime);
    }
    void VzCamera::Controller::SetOrbitPhi(const float phi) {
        GET_CM_WARN(cam_res, cm, );
        if (mode != Mode::ORBIT) return;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        bookmark.orbit.phi = phi;
        cm->jumpToBookmark(*(camutils::Bookmark<float>*) & bookmark);
    }
    float VzCamera::Controller::GetOrbitPhi() {
        GET_CM_WARN(cam_res, cm, 0.0f);
        if (mode != Mode::ORBIT) return 0.0f;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        return bookmark.orbit.phi;
    }
    void VzCamera::Controller::SetOrbitTheta(const float theta) {
        GET_CM_WARN(cam_res, cm, );
        if (mode != Mode::ORBIT) return;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        bookmark.orbit.theta = theta;
        cm->jumpToBookmark(*(camutils::Bookmark<float>*) & bookmark);
    }
    float VzCamera::Controller::GetOrbitTheta() {
        GET_CM_WARN(cam_res, cm, 0.0f);
        if (mode != Mode::ORBIT) return 0.0f;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        return bookmark.orbit.theta;
    }
    void VzCamera::Controller::SetOrbitDistance(const float distance) {
        GET_CM_WARN(cam_res, cm, );
        if (mode != Mode::ORBIT) return;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        bookmark.orbit.distance = distance;
        cm->jumpToBookmark(*(camutils::Bookmark<float>*) & bookmark);
    }
    float VzCamera::Controller::GetOrbitDistance() {
        GET_CM_WARN(cam_res, cm, 0.0f);
        if (mode != Mode::ORBIT) return 0.0f;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        return bookmark.orbit.distance;
    }
    void VzCamera::Controller::SetFlightPitch(const float pitch) {
        GET_CM_WARN(cam_res, cm, );
        if (mode != Mode::FREE_FLIGHT) return;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        bookmark.flight.pitch = pitch;
        cm->jumpToBookmark(*(camutils::Bookmark<float>*) & bookmark);
    }
    float VzCamera::Controller::GetFlightPitch() {
        GET_CM_WARN(cam_res, cm, 0.0f);
        if (mode != Mode::FREE_FLIGHT) return 0.0f;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        return bookmark.flight.pitch;
    }
    void VzCamera::Controller::SetFlightYaw(const float yaw) {
        GET_CM_WARN(cam_res, cm, );
        if (mode != Mode::FREE_FLIGHT) return;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        bookmark.flight.yaw = yaw;
        cm->jumpToBookmark(*(camutils::Bookmark<float>*) & bookmark);
    }
    float VzCamera::Controller::GetFlightYaw() {
        GET_CM_WARN(cam_res, cm, 0.0f);
        if (mode != Mode::FREE_FLIGHT) return 0.0f;
        camutils::Bookmark<float> camutilsBookmark = cm->getCurrentBookmark();
        Bookmark bookmark = *(Bookmark*) &camutilsBookmark;
        return bookmark.flight.yaw;
    }
}
