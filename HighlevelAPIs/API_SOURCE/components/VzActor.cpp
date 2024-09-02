#include "VzActor.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

namespace vzm
{
    void VzBaseActor::SetVisibleLayerMask(const uint8_t layerBits, const uint8_t maskBits)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setLayerMask(ins, layerBits, maskBits);
        UpdateTimeStamp();
    }
    void VzBaseActor::SetPriority(const uint8_t priority)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setPriority(ins, priority);
        UpdateTimeStamp();
    }
}

namespace vzm
{
    void VzActor::SetMI(const VID vidMI, const int slot)
    {
        VzMIRes* mi_res = gEngineApp->GetMIRes(vidMI);
        if (mi_res == nullptr)
        {
            backlog::post("invalid material instance!", backlog::LogLevel::Error);
            return;
        }
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        if (!actor_res->SetMI(vidMI, slot))
        {
            return;
        }
        auto& rcm = gEngine->getRenderableManager();
        utils::Entity ett_actor = utils::Entity::import(GetVID());
        auto ins = rcm.getInstance(ett_actor);
        rcm.setMaterialInstanceAt(ins, slot, mi_res->mi);
        UpdateTimeStamp();
    }
    void VzActor::SetCastShadows(const bool enabled)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setCastShadows(ins, enabled);
    }
    void VzActor::SetReceiveShadows(const bool enabled)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setReceiveShadows(ins, enabled);
    }
    void VzActor::SetScreenSpaceContactShadows(const bool enabled)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setScreenSpaceContactShadows(ins, enabled);
    }
    void VzActor::SetRenderableRes(const VID vidGeo, const std::vector<VID>& vidMIs)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        actor_res->SetGeometry(vidGeo);
        actor_res->SetMIs(vidMIs);
        gEngineApp->BuildRenderable(GetVID());
        UpdateTimeStamp();
    }
    std::vector<VID> VzActor::GetMIs()
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        return actor_res->GetMIVids();
    }
    VID VzActor::GetMI(const int slot)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        return actor_res->GetMIVid(slot);
    }
    VID VzActor::GetMaterial(const int slot)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        MInstanceVID vid_mi = actor_res->GetMIVid(slot);
        VzMIRes* mi_res = gEngineApp->GetMIRes(vid_mi);
        if (mi_res == nullptr)
        {
            return INVALID_VID;
        }
        MaterialInstance* mi = mi_res->mi;
        assert(mi);
        const Material* mat = mi->getMaterial();
        assert(mat != nullptr);
        return gEngineApp->FindMaterialVID(mat);
    }
    VID VzActor::GetGeometry()
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        return actor_res->GetGeometryVid();
    }
    void VzActor::SetMorphWeights(const float* weights, const int count)
    {
        COMP_ACTOR(rcm, ett, ins, );
        rcm.setMorphWeights(ins, weights, count);
    }
    int VzActor::GetMorphTargetCount()
    {
        COMP_ACTOR(rcm, ett, ins, 0);
        return (int)rcm.getMorphTargetCount(ins);
    }
}


namespace vzm
{
    void VzBaseSprite::EnableBillboard(const bool billboardEnabled)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(baseActor_->GetVID());
        actor_res->isBillboard = billboardEnabled;
        baseActor_->UpdateTimeStamp();
    }

    void VzBaseSprite::SetRotatation(const float rotDeg)
    {
        // TO DO
        baseActor_->UpdateTimeStamp();
    }
}

namespace vzm
{
    void buildQuadGeometry(const VID vid, const float w, const float h, const float anchorU, const float anchorV)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(vid);
        assert(actor_res->isSprite);
        if (actor_res->intrinsicVB) gEngine->destroy(actor_res->intrinsicVB);
        if (actor_res->intrinsicIB) gEngine->destroy(actor_res->intrinsicIB);

        struct SpriteVertex {
            float3 position;
            float2 uv;
        };
        float half_width = w * 0.5f;
        float half_height = h * 0.5f;
        float offset_x = (0.5f - anchorU) * half_width;
        float offset_y = (0.5f - anchorV) * half_height;
        SpriteVertex kQuadVertices[4] = {
            {{-half_width + offset_x,  half_height + offset_y, 0}, {0, 0}},
            {{ half_width + offset_x,  half_height + offset_y, 0}, {1, 0}},
            {{-half_width + offset_x, -half_height + offset_y, 0}, {0, 1}},
            {{ half_width + offset_x, -half_height + offset_y, 0}, {1, 1}} };
        uint16_t kQuadIndices[6] = { 0, 2, 1, 1, 2, 3 };

        memcpy(&actor_res->intrinsicCache[0], kQuadVertices, 80);
        memcpy(&actor_res->intrinsicCache[80], kQuadIndices, 12);

        actor_res->intrinsicVB = VertexBuffer::Builder()
            .vertexCount(4)
            .bufferCount(1)
            .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3, 0, sizeof(SpriteVertex))
            .attribute(VertexAttribute::UV0, 0, VertexBuffer::AttributeType::FLOAT2, sizeof(float3), sizeof(SpriteVertex))
            .build(*gEngine);
        actor_res->intrinsicVB->setBufferAt(*gEngine, 0,
            VertexBuffer::BufferDescriptor(&actor_res->intrinsicCache[0], 80, nullptr));

        // Create quad index buffer.
        actor_res->intrinsicIB = IndexBuffer::Builder()
            .indexCount(6)
            .bufferType(IndexBuffer::IndexType::USHORT)
            .build(*gEngine);
        actor_res->intrinsicIB->setBuffer(*gEngine, IndexBuffer::BufferDescriptor(&actor_res->intrinsicCache[80], 12, nullptr));
        Aabb aabb;
        aabb.min = { -half_width + offset_x, -half_height + offset_y, -0.5 };
        aabb.max = { half_width + offset_x, half_height + offset_y, 0.5 };

        RenderableManager::Builder builder(1);

        MaterialInstance* mi = gEngineApp->GetMIRes(actor_res->GetMIVids()[0])->mi;
        assert(mi);
        builder.material(0, mi);
        builder.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, actor_res->intrinsicVB, actor_res->intrinsicIB);

        utils::Entity ett_actor = utils::Entity::import(vid);
        builder
            .boundingBox(Box().set(aabb.min, aabb.max))
            .culling(actor_res->culling) // false
            .castShadows(actor_res->castShadow) // false
            .receiveShadows(actor_res->receiveShadow) // false
            .build(*gEngine, ett_actor);
    }

    VzSpriteActor& VzSpriteActor::SetSpriteWidth(const float w)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->spriteWidth = w;
        UpdateTimeStamp();
        return *this;
    }
    VzSpriteActor& VzSpriteActor::SetSpriteHeight(const float h)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->spriteHeight = h;
        UpdateTimeStamp();
        return *this;
    }
    VzSpriteActor& VzSpriteActor::SetAnchorU(const float u)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->anchorU = u;
        UpdateTimeStamp();
        return *this;
    }
    VzSpriteActor& VzSpriteActor::SetAnchorV(const float v)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->anchorV = v;
        UpdateTimeStamp();
        return *this;
    }

    bool VzSpriteActor::Build()
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        buildQuadGeometry(GetVID(), actor_res->spriteWidth, actor_res->spriteHeight, actor_res->anchorU, actor_res->anchorV);
        UpdateTimeStamp();
        return true;
    }

    void VzSpriteActor::SetTexture(const VID vidTexture)
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(vidTexture);
        if (tex_res->texture == nullptr) {
            backlog::post("invalid texture!", backlog::LogLevel::Error);
            return;
        }

        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        MaterialInstance* mi = gEngineApp->GetMIRes(actor_res->GetMIVid(0))->mi;
        assert(mi);

        mi->setParameter("baseColorMap", tex_res->texture, tex_res->sampler);
        //mi->getMaterial()->get
        tex_res->assignedMIs.insert(GetVID());

        UpdateTimeStamp();
    }
}


#include "../../libs/imageio/include/imageio/ImageDecoder.h"
#include <fstream>
#include <iostream>

namespace vzm
{
    void VzTextSpriteActor::SetFont(const VID vidFont)
    {
        VzFontRes* font_res = gEngineApp->GetFontRes(vidFont);
        if (font_res->ftFace_ == nullptr)
        {
            backlog::post("invalid font!", backlog::LogLevel::Error);
            return;
        }
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        actor_res->textField.typesetter.textFormat.font = vidFont;
        UpdateTimeStamp();
    }

    VzTextSpriteActor& VzTextSpriteActor::SetText(const std::string& text) {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->textField.typesetter.text = std::wstring(text.begin(), text.end());
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetText(const std::wstring& text)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->textField.typesetter.text = text;
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetAnchorU(const float anchorU)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->anchorU = anchorU;
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetAnchorV(const float anchorV)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->anchorV = anchorV;
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetColor(const float color[4])
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->textField.textColor[0] = color[0];
        actor_res->textField.textColor[1] = color[1];
        actor_res->textField.textColor[2] = color[2];
        actor_res->textField.textColor[3] = color[3];
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetFontHeight(const float fontHeight)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->fontHeight = fontHeight;
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetMaxWidth(const float maxWidth)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->spriteWidth = maxWidth;
        UpdateTimeStamp();
        return *this;
    }

    VzTextSpriteActor& VzTextSpriteActor::SetTextAlign(const TEXT_ALIGN textAlign)
    {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);
        actor_res->textField.typesetter.textFormat.textAlign = textAlign;
        UpdateTimeStamp();
        return *this;
    }

    void VzTextSpriteActor::Build() {
        VzActorRes* actor_res = gEngineApp->GetActorRes(GetVID());
        assert(actor_res->isSprite);

        FontVID font = actor_res->textField.typesetter.textFormat.font;
        VzFontRes* font_res = gEngineApp->GetFontRes(font);
        if ((font == INVALID_VID) || (font_res->ftFace_ == nullptr))
        {
            backlog::post("invalid font!", backlog::LogLevel::Error);
            return;
        }
        // TO DO //
        // generate a text image (rgba, backgournd alpha is zero) from "text"
        // this is supposed to update the following
        //      * image width and height in pixels : text_image_w, text_image_h
        //      * unsigned char* as image (rgba) buffer pointer (the allocation will be owned by VzTextSpriteActor)
        VzTypesetter& typesetter = actor_res->textField.typesetter;
        if (typesetter.text.empty()) typesetter.text = L" ";
        if (actor_res->spriteWidth > 1.f)
        {
            typesetter.fixedWidth = (int32_t) (actor_res->spriteWidth / actor_res->fontHeight * (float) font_res->GetLineHeight());
        }
        else
        {
            typesetter.fixedWidth = 0;
        }
        typesetter.Typeset();

        MaterialInstance* mi = gEngineApp->GetMIRes(actor_res->GetMIVids()[0])->mi;
        TextureSampler sampler;
        sampler.setMagFilter(TextureSampler::MagFilter::LINEAR);
        sampler.setMinFilter(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR);
        sampler.setWrapModeS(TextureSampler::WrapMode::REPEAT);
        sampler.setWrapModeT(TextureSampler::WrapMode::REPEAT); 
        mi->setParameter("baseColorFactor", (filament::RgbaType) RgbaType::LINEAR, *(float4*) actor_res->textField.textColor);
        mi->setParameter("textTexture", typesetter.texture, sampler);
        if (actor_res->intrinsicTexture) gEngine->destroy(actor_res->intrinsicTexture);
        actor_res->intrinsicTexture = typesetter.texture;

        size_t text_image_w = typesetter.texture->getWidth();
        size_t text_image_h = typesetter.texture->getHeight();

        float w, h;
        if (actor_res->spriteWidth > 1.f)
        {
            w = actor_res->spriteWidth;
            h = actor_res->spriteWidth / text_image_w * text_image_h;
        }
        else
        {
            h = actor_res->fontHeight * actor_res->textField.typesetter.linesWidth.size();
            w = h / text_image_h * text_image_w;
        }
        //if (actor_res->intrinsicVB) gEngine->destroy(actor_res->intrinsicVB);
        //if (actor_res->intrinsicIB) gEngine->destroy(actor_res->intrinsicIB);
        buildQuadGeometry(GetVID(), w, h, actor_res->anchorU, actor_res->anchorV);

        UpdateTimeStamp();
    }
}
