#include "VzLight.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

extern Engine* gEngine;
extern vzm::VzEngineApp gEngineApp;

namespace vzm
{
    //void VzLight::SetType(const Type type)
    //{
    //    COMP_LIGHT(lcm, ett, ins, );
    //    LightManager::Builder builder((LightManager::Type)type);
    //    for (unsigned int channel = 0; channel < 8; channel++)
    //    {
    //        builder.lightChannel(channel, lcm.getLightChannel(ins, channel));
    //    }
    //    builder.castShadows(lcm.isShadowCaster(ins));
    //    builder.shadowOptions(lcm.getShadowOptions(ins));
    //    builder.position(lcm.getPosition(ins));
    //    builder.direction(lcm.getDirection(ins));
    //    builder.color(lcm.getColor(ins));
    //    builder.intensityCandela(lcm.getIntensity(ins));
    //    builder.falloff(lcm.getFalloff(ins));
    //    builder.spotLightCone(lcm.getSpotLightInnerCone(ins), lcm.getSpotLightOuterCone(ins));
    //    builder.sunAngularRadius(lcm.getSunAngularRadius(ins));
    //    builder.sunHaloSize(lcm.getSunHaloSize(ins));
    //    builder.sunHaloFalloff(lcm.getSunHaloFalloff(ins));
    //    builder.build(*gEngine, ett);
    //    UpdateTimeStamp();
    //}
    //VzLight::Type VzLight::GetType() const
    //{
    //    COMP_LIGHT(lcm, ett, ins, Type::DIRECTIONAL);
    //    return (VzLight::Type)lcm.getType(ins);
    //}


    void VzBaseLight::SetLightChannel(unsigned int channel, bool enable)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setLightChannel(ins, channel, enable);
        UpdateTimeStamp();
    }
    bool VzBaseLight::GetLightChannel(unsigned int channel) const
    {
        COMP_LIGHT(lcm, ett, ins, false);
        return lcm.getLightChannel(ins, channel);
    }
    void VzBaseLight::SetPosition(const float position[3])
    {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _position = *(float3*)position;
        lcm.setPosition(ins, _position);
        UpdateTimeStamp();
    }
    void VzBaseLight::GetPosition(float position[3])
    {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _position = lcm.getPosition(ins);
        if (position) *(float3*)position = float3(_position);
    }
    void VzBaseLight::SetDirection(const float direction[3])
    {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _direction = *(float3*)direction;
        lcm.setDirection(ins, _direction);
        UpdateTimeStamp();
    }
    void VzBaseLight::GetDirection(float direction[3])
    {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _direction = lcm.getDirection(ins);
        if (direction) *(float3*)direction = float3(_direction);
    }
    void VzBaseLight::SetColor(const float color[3]) {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _color = *(float3*)color;
        lcm.setColor(ins, _color);
    }
    void VzBaseLight::GetColor(float color[3]) {
        COMP_LIGHT(lcm, ett, ins, );
        float3 _color = lcm.getColor(ins);
        if (color) *(float3*)color = float3(_color);
    }
    void VzBaseLight::SetIntensity(const float intensity) {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setIntensityCandela(ins, intensity);
        UpdateTimeStamp();
    }
    float VzBaseLight::GetIntensity() const
    {
        COMP_LIGHT(lcm, ett, ins, 100000.0f);
        return lcm.getIntensity(ins);
    }

    void VzBaseLight::SetShadowOptions(ShadowOptions const& options)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setShadowOptions(ins, (LightManager::ShadowOptions const&)options);
        UpdateTimeStamp();
    }
    VzBaseLight::ShadowOptions const* VzBaseLight::GetShadowOptions() const
    {
        COMP_LIGHT(lcm, ett, ins, nullptr);
        return (VzBaseLight::ShadowOptions const*)&lcm.getShadowOptions(ins);
    }
    void VzBaseLight::SetShadowCaster(bool shadowCaster)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setShadowCaster(ins, shadowCaster);
        UpdateTimeStamp();
    }
    bool VzBaseLight::IsShadowCaster() const
    {
        COMP_LIGHT(lcm, ett, ins, false);
        return lcm.isShadowCaster(ins);
    }

#define COMP_LIGHT_BRANCH(BASE, COMP, ENTITY, INS, FAILRET)  auto & COMP = gEngine->getLightManager(); Entity ENTITY = Entity::import(BASE->GetVID()); if (ENTITY.isNull()) return FAILRET; auto INS = COMP.getInstance(ENTITY);
    void VzBaseSpotLight::SetFalloff(const float radius)
    {
        COMP_LIGHT_BRANCH(baseLight_, lcm, ett, ins, );
        lcm.setFalloff(ins, radius);
        baseLight_->UpdateTimeStamp();
    }
    float VzBaseSpotLight::GetFalloff() const
    {
        COMP_LIGHT_BRANCH(baseLight_, lcm, ett, ins, 1.0);
        return lcm.getFalloff(ins);
    }

    void VzPointLight::SetFalloff(const float radius)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setFalloff(ins, radius);
        UpdateTimeStamp();
    }
    float VzPointLight::GetFalloff() const
    {
        COMP_LIGHT(lcm, ett, ins, 1.0f);
        return lcm.getFalloff(ins);
    }

    void VzBaseSpotLight::SetSpotLightCone(const float inner, const float outer)
    {
        COMP_LIGHT_BRANCH(baseLight_, lcm, ett, ins, );
        lcm.setSpotLightCone(ins, inner, outer);
        baseLight_->UpdateTimeStamp();
    }
    float VzBaseSpotLight::GetSpotLightOuterCone() const
    {
        COMP_LIGHT_BRANCH(baseLight_, lcm, ett, ins, VZ_PIDIV4);
        return lcm.getSpotLightOuterCone(ins);
    }
    float VzBaseSpotLight::GetSpotLightInnerCone() const
    {
        COMP_LIGHT_BRANCH(baseLight_, lcm, ett, ins, VZ_PIDIV4 * 0.75f);
        return lcm.getSpotLightInnerCone(ins);
    }

    void VzSunLight::SetSunAngularRadius(const float angularRadius)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setSunAngularRadius(ins, angularRadius);
        UpdateTimeStamp();
    }
    float VzSunLight::GetSunAngularRadius() const
    {
        COMP_LIGHT(lcm, ett, ins, 0.00951f);
        return lcm.getSunAngularRadius(ins);
    }
    void VzSunLight::SetSunHaloSize(const float haloSize)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setSunHaloSize(ins, haloSize);
        UpdateTimeStamp();
    }
    float VzSunLight::GetSunHaloSize() const
    {
        COMP_LIGHT(lcm, ett, ins, 10.0f);
        return lcm.getSunHaloSize(ins);
    }
    void VzSunLight::SetSunHaloFalloff(const float haloFalloff)
    {
        COMP_LIGHT(lcm, ett, ins, );
        lcm.setSunHaloFalloff(ins, haloFalloff);
        UpdateTimeStamp();
    }
    float VzSunLight::GetSunHaloFalloff() const
    {
        COMP_LIGHT(lcm, ett, ins, 80.0f);
        return lcm.getSunHaloFalloff(ins);
    }
}
