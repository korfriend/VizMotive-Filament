#pragma once
#include "../VizComponentAPIs.h"
#include "VzMaterial.h"

namespace vzm
{
    struct API_EXPORT VzMI : VzResource
    {
        VzMI(const VID vid, const std::string& originFrom)
            : VzResource(vid, originFrom, "VzMI", RES_COMPONENT_TYPE::MATERIALINSTANCE) {}

        enum class TransparencyMode : uint8_t {
            DEFAULT = 0, // following the rasterizer state
            TWO_PASSES_ONE_SIDE,
            TWO_PASSES_TWO_SIDES
        };

        void SetDoubleSided(const bool doubleSided);
        bool IsDoubleSided() const;
        void SetTransparencyMode(const TransparencyMode tMode);
        TransparencyMode GetTransparencyMode() const;
        bool SetParameter(const std::string& name, const vzm::UniformType vType, const void* v);
        bool SetParameter(const std::string& name, const vzm::RgbType vType, const float* v);
        bool SetParameter(const std::string& name, const vzm::RgbaType vType, const float* v);
        bool GetParameter(const std::string& name, const vzm::UniformType vType, const void* v);
        VID GetTexture(const std::string& name);
        bool SetTexture(const std::string& name, const VID vidTexture,
                  const bool retainSampler = true);
        bool GetUvTransform(const std::string& name, float offset[2], float& rotation, float scale[2]) const;
        bool SetUvTransform(const std::string& name, const float offset[2], const float rotation, const float scale[2]);
        
        VID GetMaterial();
        bool SetMaterial(const VID vidMaterial);
    };
}
