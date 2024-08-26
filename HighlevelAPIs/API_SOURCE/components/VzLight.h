#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzBaseLight : VzSceneComp
    {
        VzBaseLight(const VID vid, const std::string& originFrom, const std::string& typeName, const SCENE_COMPONENT_TYPE scenecompType)
            : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

        void SetLightChannel(unsigned int channel, bool enable = true);
        bool GetLightChannel(unsigned int channel) const;

        void SetPosition(const float position[3]);
        void GetPosition(float position[3]);

        void SetDirection(const float direction[3]);
        void GetDirection(float direction[3]);

        void SetColor(const float color[3]);
        void GetColor(float color[3]);

        void SetIntensity(const float intensity);
        float GetIntensity() const;

        struct ShadowOptions {
            uint32_t mapSize = 1024;
            uint8_t shadowCascades = 1;
            float cascadeSplitPositions[3] = { 0.125f, 0.25f, 0.50f };
            float constantBias = 0.001f;
            float normalBias = 1.0f;
            float shadowFar = 0.0f;
            float shadowNearHint = 1.0f;
            float shadowFarHint = 100.0f;
            bool stable = false;
            bool lispsm = true;
            float polygonOffsetConstant = 0.5f;
            float polygonOffsetSlope = 2.0f;
            bool screenSpaceContactShadows = false;
            uint8_t stepCount = 8;
            float maxShadowDistance = 0.3f;
            struct Vsm {
                bool elvsm = false;
                float blurWidth = 0.0f;
            } vsm;
            float shadowBulbRadius = 0.02f;
            float transform[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        };
        void SetShadowOptions(ShadowOptions const& options);
        ShadowOptions const* GetShadowOptions() const;

        void SetShadowCaster(bool shadowCaster);
        bool IsShadowCaster() const;
    };

    struct API_EXPORT VzBaseSpotLight
    {
    private:
        VzBaseLight* baseLight_;
    public:
        VzBaseSpotLight(VzBaseLight* baseLight) : baseLight_(baseLight) {};

        void SetSpotLightCone(const float inner, const float outer);
        float GetSpotLightOuterCone() const;
        float GetSpotLightInnerCone() const;

        void SetFalloff(const float radius);
        float GetFalloff() const;
    };

    struct API_EXPORT VzSpotLight : VzBaseLight, VzBaseSpotLight
    {
        VzSpotLight(const VID vid, const std::string& originFrom)
            : VzBaseLight(vid, originFrom, "VzSpotLight", SCENE_COMPONENT_TYPE::LIGHT_SPOT), VzBaseSpotLight(this) {}
    };

    struct API_EXPORT VzFocusedSpotLight : VzBaseLight, VzBaseSpotLight
    {
        VzFocusedSpotLight(const VID vid, const std::string& originFrom)
            : VzBaseLight(vid, originFrom, "VzFocusedSpotLight", SCENE_COMPONENT_TYPE::LIGHT_FOCUSED_SPOT), VzBaseSpotLight(this) {}
    };

    struct API_EXPORT VzSunLight : VzBaseLight
    {
        VzSunLight(const VID vid, const std::string& originFrom)
            : VzBaseLight(vid, originFrom, "VzSunLight", SCENE_COMPONENT_TYPE::LIGHT_SUN) {}

        void SetSunAngularRadius(const float angularRadius);
        float GetSunAngularRadius() const;

        void SetSunHaloSize(const float haloSize);
        float GetSunHaloSize() const;

        void SetSunHaloFalloff(const float haloFalloff);
        float GetSunHaloFalloff() const;
    };

    struct API_EXPORT VzDirectionalLight : VzBaseLight
    {
        VzDirectionalLight(const VID vid, const std::string& originFrom)
            : VzBaseLight(vid, originFrom, "VzDirectionalLight", SCENE_COMPONENT_TYPE::LIGHT_DIRECTIONAL) {}
    };

    struct API_EXPORT VzPointLight : VzBaseLight
    {
        VzPointLight(const VID vid, const std::string& originFrom)
            : VzBaseLight(vid, originFrom, "VzPointLight", SCENE_COMPONENT_TYPE::LIGHT_POINT) {}

        void SetFalloff(const float radius);
        float GetFalloff() const;
    };
}
