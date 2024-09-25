#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzBaseActor : VzSceneComp
    {
        VzBaseActor(const VID vid, const std::string & originFrom, const std::string & typeName, const SCENE_COMPONENT_TYPE scenecompType)
            : VzSceneComp(vid, originFrom, typeName, scenecompType) {}

        uint8_t GetVisibleLayerMask() const;
        void SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits);
        bool IsVisible() const;
        void SetVisible(const bool visible);
        bool IsPostProcessingEnabled() const;
        void SetPostProcessingEnabled(const bool enabled);
        uint8_t GetPriority() const;
        void SetPriority(const uint8_t priority);
        void GetAxisAlignedBoundingBox(float min[3], float max[3]);
    };

    struct API_EXPORT VzActor : VzBaseActor
    {
        VzActor(const VID vid, const std::string& originFrom)
            : VzBaseActor(vid, originFrom, "VzActor", SCENE_COMPONENT_TYPE::ACTOR) {}

        void SetRenderableRes(const VID vidGeo, const std::vector<VID>& vidMIs);
        void SetMI(const VID vidMI, const int slot = 0);

        void SetCastShadows(const bool enabled);
        void SetReceiveShadows(const bool enabled);
        void SetScreenSpaceContactShadows(const bool enabled);

        std::vector<VID> GetMIs();
        VID GetMI(const int slot = 0);
        VID GetMaterial(const int slot = 0);
        VID GetGeometry();

        size_t GetMorphWeights(std::vector<float>& weights);
        void SetMorphWeights(const float* weights, const int count);
        int GetMorphTargetCount();
    };

    struct API_EXPORT VzBaseSprite
    {
    private:
        VzBaseActor* baseActor_;
    public:
        VzBaseSprite(VzBaseActor* baseActor) : baseActor_(baseActor) {};

        void EnableBillboard(const bool billboardEnabled);

        // The rotation of the sprite in degrees. Default is 0.
        void SetRotation(const float rotDeg);

        // Sprite Helpers...
        static void ComputeScreenSpriteParams(
            const uint32_t x, const uint32_t y,
            const uint32_t spriteW, const uint32_t spriteH,
            const float u, const float v,
            VID camera, VID renderer,
            float& w, float& h, float p[3]);
    };

    struct API_EXPORT VzSpriteActor : VzBaseActor, VzBaseSprite
    {
        VzSpriteActor(const VID vid, const std::string & originFrom)
            : VzBaseActor(vid, originFrom, "VzSpriteActor", SCENE_COMPONENT_TYPE::SPRITE_ACTOR), VzBaseSprite(this) {}

        // The sprite's anchor point, and the point around which the sprite rotates. 
        // A value of (0.5, 0.5) corresponds to the midpoint of the sprite. 
        // A value of (0, 0) corresponds to the lower left corner of the sprite. The default is (0.5, 0.5).
        // basic local frame is x:(1, 0, 0), y:(0, 1, 0), z:(0, 0, 1), sprite plane is defined on xy-plane
        float GetSpriteWidth();
        float GetSpriteHeight();
        float GetAnchorU();
        float GetAnchorV();
        VzSpriteActor& SetSpriteWidth(const float w = 1.f);
        VzSpriteActor& SetSpriteHeight(const float h = 1.f);
        VzSpriteActor& SetAnchorU(const float u = 0.5f);
        VzSpriteActor& SetAnchorV(const float v = 0.5f);
        bool Build();
        //void SetAnchorPoint(const float u = 0.5f, const float v = 0.5f);
        //void SetSize(const float w = 1.f, const float h = 1.f);
        VID GetTexture();
        void SetTexture(const VID vidTexture);
    };

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

    struct API_EXPORT VzTextSpriteActor : VzBaseActor, VzBaseSprite
    {
        VzTextSpriteActor(const VID vid, const std::string & originFrom)
            : VzBaseActor(vid, originFrom, "VzTextSpriteActor", SCENE_COMPONENT_TYPE::TEXT_SPRITE_ACTOR), VzBaseSprite(this) {}

        VID GetFont();
        void SetFont(const VID vidFont);

        // The sprite's anchor point, and the point around which the sprite rotates. 
        // A value of (0.5, 0.5) corresponds to the midpoint of the sprite. 
        // A value of (0, 0) corresponds to the lower left corner of the sprite. The default is (0.5, 0.5).
        // basic local frame is x:(1, 0, 0), y:(0, 1, 0), z:(0, 0, 1), sprite plane is defined on xy-plane
        std::string GetText();
        std::wstring GetTextW();
        float GetAnchorU();
        float GetAnchorV();
        void GetColor(float color[4]);
        float GetFontHeight();
        float GetMaxWidth();
        TEXT_ALIGN GetTextAlign();
        VzTextSpriteActor& SetText(const std::string& text);
        VzTextSpriteActor& SetTextW(const std::wstring& text);
        VzTextSpriteActor& SetAnchorU(const float anchorU);
        VzTextSpriteActor& SetAnchorV(const float anchorV);
        VzTextSpriteActor& SetColor(const float color[4]);
        VzTextSpriteActor& SetFontHeight(const float fontHeight);
        VzTextSpriteActor& SetMaxWidth(const float maxWidth);
        VzTextSpriteActor& SetTextAlign(const TEXT_ALIGN textAlign);
        void Build();
    };
}
