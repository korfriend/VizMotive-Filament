#include "VzMI.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

extern std::vector<std::string> gMProp;

namespace vzm
{
    void VzMI::SetDoubleSided(const bool doubleSided)
    {
        COMP_MI(mi, mi_res, );
        mi->setDoubleSided(doubleSided);
        UpdateTimeStamp();
    }

    bool VzMI::IsDoubleSided() const
    {
        COMP_MI(mi, mi_res, false);
        return mi->isDoubleSided();
    }

    void VzMI::SetTransparencyMode(const TransparencyMode tMode)
    {
        COMP_MI(mi, mi_res, );
        mi->setTransparencyMode((filament::TransparencyMode)tMode);
        UpdateTimeStamp();
    }

    VzMI::TransparencyMode VzMI::GetTransparencyMode() const
    {
        COMP_MI(mi, mi_res, TransparencyMode::DEFAULT);
        return (TransparencyMode)mi->getTransparencyMode();
    }
#define SET_PARAM_COMP(COMP, RES, M_RES, FAILRET) COMP_MI(COMP, RES, FAILRET); VzMaterialRes* M_RES = gEngineApp->GetMaterialRes(RES->vidMaterial); if (!M_RES->allowedParamters.contains(name)) return FAILRET;
    bool VzMI::SetParameter(const std::string& name, const vzm::UniformType vType, const void* v)
    {
        SET_PARAM_COMP(mi, mi_res, m_res, false);

        const char* cstr = name.c_str();
        switch (vType)
        {
        case vzm::UniformType::BOOL: mi->setParameter(cstr, *(bool*)v); break;
        case vzm::UniformType::BOOL2: mi->setParameter(cstr, *(math::bool2*)v); break;
        case vzm::UniformType::BOOL3: mi->setParameter(cstr, *(math::bool3*)v); break;
        case vzm::UniformType::BOOL4: mi->setParameter(cstr, *(math::bool4*)v); break;
        case vzm::UniformType::FLOAT: mi->setParameter(cstr, *(float*)v); break;
        case vzm::UniformType::FLOAT2: mi->setParameter(cstr, *(math::float2*)v); break;
        case vzm::UniformType::FLOAT3: mi->setParameter(cstr, *(math::float3*)v); break;
        case vzm::UniformType::FLOAT4: mi->setParameter(cstr, *(math::float4*)v); break;
        case vzm::UniformType::INT: mi->setParameter(cstr, *(int*)v); break;
        case vzm::UniformType::INT2: mi->setParameter(cstr, *(math::int2*)v); break;
        case vzm::UniformType::INT3: mi->setParameter(cstr, *(math::int3*)v); break;
        case vzm::UniformType::INT4: mi->setParameter(cstr, *(math::int4*)v); break;
        case vzm::UniformType::UINT: mi->setParameter(cstr, *(uint*)v); break;
        case vzm::UniformType::UINT2: mi->setParameter(cstr, *(math::uint2*)v); break;
        case vzm::UniformType::UINT3: mi->setParameter(cstr, *(math::uint3*)v); break;
        case vzm::UniformType::UINT4: mi->setParameter(cstr, *(math::uint4*)v); break;
        case vzm::UniformType::MAT3: mi->setParameter(cstr, *(math::mat3f*)v); break;
        case vzm::UniformType::MAT4: mi->setParameter(cstr, *(math::mat4f*)v); break;
        case vzm::UniformType::STRUCT: 
        default:
            return false;
        }
        UpdateTimeStamp();
        return true;
    }

    bool VzMI::SetParameter(const std::string& name, const vzm::RgbType vType, const float* v)
    {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        mi->setParameter(name.c_str(), (filament::RgbType)vType, *(math::float3*)v);
        UpdateTimeStamp();
        return true;
    }

    bool VzMI::SetParameter(const std::string& name, const vzm::RgbaType vType, const float* v)
    {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        mi->setParameter(name.c_str(), (filament::RgbaType)vType, *(math::float4*)v);
        UpdateTimeStamp();
        return true;
    }

    bool VzMI::GetParameter(const std::string& name, const vzm::UniformType vType, const void* v)
    {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        const char* cstr = name.c_str();
        switch (vType)
        {
        //case vzm::UniformType::BOOL: *(bool*)v = mi->getParameter<bool>(cstr); break;
        //case vzm::UniformType::BOOL2: *(math::bool2*)v = mi->getParameter<math::bool2>(cstr); break;
        //case vzm::UniformType::BOOL3: *(math::bool3*)v = mi->getParameter<math::bool3>(cstr); break;
        //case vzm::UniformType::BOOL4: *(math::bool4*)v = mi->getParameter<math::bool4>(cstr); break;
        case vzm::UniformType::FLOAT: *(float*)v = mi->getParameter<float>(cstr); break;
        case vzm::UniformType::FLOAT2: *(math::float2*)v = mi->getParameter<math::float2>(cstr); break;
        case vzm::UniformType::FLOAT3: *(math::float3*)v = mi->getParameter<math::float3>(cstr); break;
        case vzm::UniformType::FLOAT4: *(math::float4*)v = mi->getParameter<math::float4>(cstr); break;
        case vzm::UniformType::INT: *(int*)v = mi->getParameter<int>(cstr); break;
        case vzm::UniformType::INT2: *(math::int2*)v = mi->getParameter<math::int2>(cstr); break;
        case vzm::UniformType::INT3: *(math::int3*)v = mi->getParameter<math::int3>(cstr); break;
        case vzm::UniformType::INT4: *(math::int4*)v = mi->getParameter<math::int4>(cstr); break;
        case vzm::UniformType::UINT: *(uint*)v = mi->getParameter<uint>(cstr); break;
        case vzm::UniformType::UINT2: *(math::uint2*)v = mi->getParameter<math::uint2>(cstr); break;
        case vzm::UniformType::UINT3: *(math::uint3*)v = mi->getParameter<math::uint3>(cstr); break;
        case vzm::UniformType::UINT4: *(math::uint4*)v = mi->getParameter<math::uint4>(cstr); break;
        case vzm::UniformType::MAT3: *(math::mat3f*)v = mi->getParameter<math::mat3f>(cstr); break;
        //case vzm::UniformType::MAT4: *(math::mat4f*)v = mi->getParameter<math::mat4f>(cstr); break;
        case vzm::UniformType::STRUCT:
        default:
            return false;
        }
        return true;
    }

    bool VzMI::SetTexture(const std::string& name, const VID vidTexture,
                          const bool retainSampler) {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(vidTexture);
        
        if (tex_res->texture == nullptr) {
          return false;
        }
        
        if (retainSampler) {
          VID prev_texture = GetTexture(name);
          VzTextureRes* prev_tex_res = gEngineApp->GetTextureRes(prev_texture);
        
          if (prev_tex_res->texture == nullptr) {
            return false;
          }
        
          tex_res->sampler = prev_tex_res->sampler;
        }
        
        mi->setParameter(name.c_str(), tex_res->texture, tex_res->sampler);
        
        mi_res->texMap[name] = vidTexture;
        tex_res->assignedMIs.insert(GetVID());
        
        UpdateTimeStamp();
        return true;
    }

    bool VzMI::GetUvTransform(const std::string& name, float offset[2], float& rotation, float scale[2]) const {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        math::mat3f uvmat = mi->getParameter<math::mat3f>(name.c_str());
        offset[0] = uvmat[0][2];
        offset[1] = uvmat[1][2];
        scale[0] = sqrt(uvmat[0][0] * uvmat[0][0] + uvmat[1][0] * uvmat[1][0]);
        scale[1] = sqrt(uvmat[0][1] * uvmat[0][1] + uvmat[1][1] * uvmat[1][1]);
        rotation = atan2(uvmat[1][0], uvmat[0][0]);
        return true;
    }

    bool VzMI::SetUvTransform(const std::string& name, const float offset[2], const float rotation, const float scale[2]) {
        SET_PARAM_COMP(mi, mi_res, m_res, false);
        float tx = offset[0];
        float ty = offset[1];
        float sx = scale[0];
        float sy = scale[1];
        float c = cos(rotation);
        float s = sin(rotation);
        mi->setParameter(name.c_str(), math::mat3f(sx * c, sx * s, tx, -sy * s, sy * c, ty, 0.0f, 0.0f, 1.0f));
        UpdateTimeStamp();
        return true;
    }

    VID VzMI::GetTexture(const std::string& name)
    {
        SET_PARAM_COMP(mi, mi_res, m_res, INVALID_VID);
        auto it = mi_res->texMap.find(name);
        if (it == mi_res->texMap.end())
        {
            return INVALID_VID;
        }
        return it->second;
    }

    VID VzMI::GetMaterial()
    {
        COMP_MI(mi, mi_res, INVALID_VID);
        return mi_res->vidMaterial;
    }

    bool VzMI::SetMaterial(const VID vidMaterial)
    {
      VzMaterialRes* mat_res = gEngineApp->GetMaterialRes(vidMaterial);
      VzMIRes* mi_res = gEngineApp->GetMIRes(GetVID());
      MaterialInstance* mi = mat_res->material->createInstance(GetName().c_str());
      mi_res->vidMaterial = vidMaterial;
      mi_res->mi = mi;
      
      return true;
    }
}
