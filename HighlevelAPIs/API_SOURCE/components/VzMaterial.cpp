#include "VzMaterial.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;
extern gltfio::MaterialProvider* gMaterialProvider;

namespace vzm
{
    bool VzMaterial::IsStandardMaterial() const
    {
        COMP_MAT(material, m_res, true);
        return m_res->isStandardType;
    }

    size_t VzMaterial::GetAllowedParameters(std::map<std::string, ParameterInfo>& paramters)
    {
        COMP_MAT(material, m_res, 0);

        for (auto& kv : m_res->allowedParamters)
        {
            ParameterInfo pi;

            pi.name = kv.first.c_str();
            pi.isSampler = kv.second.isSampler;
            pi.isSubpass = kv.second.isSubpass;
            pi.type = (vzm::UniformType)kv.second.type;
            pi.samplerType = (vzm::SamplerType)kv.second.samplerType;
            pi.subpassType = (vzm::SubpassType)kv.second.subpassType;
            pi.count = kv.second.count;
            pi.precision = (vzm::Precision)kv.second.precision;

            paramters[kv.first] = pi;
        }
        return m_res->allowedParamters.size();
    }

    bool VzMaterial::GetStandardMaterialKey(MaterialKey& materialKey) {
        COMP_MAT(material, m_res, false);

        if (!m_res->isStandardType)
        {
            backlog::post("Material (" + GetName() + ") is NOT standard material!", backlog::LogLevel::Error);
            return false;
        }

        bool unlit = material->getShading() == Shading::UNLIT;

        materialKey = MaterialKey {
            .unlit = unlit,
            .hasBaseColorTexture = material->hasParameter("baseColorMap"),
            .hasNormalTexture = material->hasParameter("normalMap"),
            .hasOcclusionTexture = material->hasParameter("occlusionMap"),
            .hasEmissiveTexture = material->hasParameter("emissiveMap"),
            .hasClearCoatTexture = material->hasParameter("clearCoatMap"),
            .hasClearCoatRoughnessTexture =
                material->hasParameter("clearCoatRoughnessMap"),
            .hasClearCoatNormalTexture =
                material->hasParameter("clearCoatNormalMap"),
            .hasClearCoat = material->hasParameter("clearCoatFactor"),
            .hasTransmission = material->hasParameter("transmissionFactor"),
            .hasTextureTransforms = material->hasParameter("baseColorUvMatrix"),
            .hasTransmissionTexture = material->hasParameter("transmissionMap"),
            .hasSheenColorTexture = material->hasParameter("sheenColorMap"),
            .hasSheenRoughnessTexture =
                material->hasParameter("sheenRoughnessMap"),
            .hasVolumeThicknessTexture =
                material->hasParameter("volumeThicknessMap"),
            .hasSheen = material->hasParameter("sheenColorFactor"),
            .hasIOR = material->hasParameter("ior"),
            .hasVolume = material->hasParameter("volumeThicknessFactor"),
            .hasSpecular = material->hasParameter("specularColorFactor"),
            .hasSpecularTexture = material->hasParameter("specularMap"),
            .hasSpecularColorTexture =
                material->hasParameter("specularColorMap"),
        };

        if (material->hasParameter("metallicRoughnessMap")) {
            materialKey.hasMetallicRoughnessTexture = true;
        }
        BlendingMode blending = material->getBlendingMode();
        if (blending == BlendingMode::OPAQUE) {
            materialKey.alphaMode = AlphaMode::OPAQUE;
        } else if (blending == BlendingMode::MASKED) {
            materialKey.alphaMode = AlphaMode::MASK;
        } else {
            materialKey.alphaMode = AlphaMode::BLEND;
        }

        return true;
    }
    
    bool VzMaterial::SetStandardMaterialByKey(const vzm::VzMaterial::MaterialKey& materialKey) {
      VzMaterialRes* mat_res = gEngineApp->GetMaterialRes(GetVID());
      if (mat_res->material) {
        gEngine->destroy(mat_res->material);
        mat_res->material = nullptr;
        mat_res->allowedParamters.clear();
      }

      UvMap uvmap;
      Material* material = gMaterialProvider->getMaterial((filament::gltfio::MaterialKey*)&materialKey, &uvmap, GetName().c_str());
      
      std::vector<Material::ParameterInfo> params(material->getParameterCount());

      material->getParameters(&params[0], params.size());
      
      for (auto& param : params) {
        assert(!mat_res->allowedParamters.contains(param.name));
        mat_res->allowedParamters[param.name] = param;
      }

      mat_res->material = material;

      return true;
    }

    }  // namespace vzm
