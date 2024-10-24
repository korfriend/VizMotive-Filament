#include "VzTexture.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

#include "../../libs/imageio/include/imageio/ImageDecoder.h"

#include <fstream>
#include <iostream>
#include <stb_image.h>

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

using namespace image;
namespace vzm
{
#define ASYNCCHECK if (tex_res->isAsyncLocked) { backlog::post("Texture (" + GetName() + ") is under asynchronuous loading, so not allowed be to update!", backlog::LogLevel::Error); return false; }

    bool VzTexture::ReadImage(const std::string& fileName, const bool isLinear, const bool generateMIPs)
    {
        // need 'safe check'
        // check if the texture is async texture
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        ASYNCCHECK;

        bool isNew = false;
        if (tex_res->texture) {
            isNew = true;
            gEngine->destroy(tex_res->texture);
            tex_res->texture = nullptr;
        }

        Path file_name(fileName);
        if (!file_name.exists()) {
            backlog::post("The input image does not exist: " + fileName, backlog::LogLevel::Error);
            return false;
        }

        tex_res->fileName = fileName;

        if ((fileName.rfind(".jpg") != std::string::npos) ||
            (fileName.rfind(".jpeg") != std::string::npos)) {
            std::ifstream inputStream(file_name, std::ios::binary);

            std::vector<uint8_t> inputBuffer;
            inputStream.seekg(0, std::ios::end);
            inputBuffer.reserve((size_t)inputStream.tellg());
            inputStream.seekg(0, std::ios::beg);
            inputBuffer.assign((std::istreambuf_iterator<char>(inputStream)),
                        std::istreambuf_iterator<char>());
            int w, h, n;
            unsigned char* data = stbi_load_from_memory(
                  inputBuffer.data(), inputBuffer.size(), &w, &h, &n, 3);

            inputStream.close();

            Texture* texture = Texture::Builder()
                                   .width(uint32_t(w))
                                   .height(uint32_t(h))
                                   .levels(0xff)
                                   .format(Texture::InternalFormat::RGB8)
                                   .sampler(Texture::Sampler::SAMPLER_2D)
                                   .build(*gEngine);

            Texture::PixelBufferDescriptor buffer(
                data, size_t(w * h * 3), Texture::Format::RGB,
                Texture::Type::UBYTE,
                (Texture::PixelBufferDescriptor::Callback)&stbi_image_free);
            texture->setImage(*gEngine, 0, std::move(buffer));
  
            tex_res->texture = texture;
        } else {
            std::ifstream inputStream(file_name, std::ios::binary);

            ImageDecoder::ColorSpace colorSpace =
                isLinear ? ImageDecoder::ColorSpace::LINEAR
                         : ImageDecoder::ColorSpace::SRGB;
            image::LinearImage* image = new LinearImage(ImageDecoder::decode(inputStream, file_name, colorSpace));

            if (!image->isValid()) {
                backlog::post("The input image is invalid:: " + fileName, backlog::LogLevel::Error);
                return false;
            }

            inputStream.close();

            uint32_t channels = image->getChannels();
            uint32_t w = image->getWidth();
            uint32_t h = image->getHeight();
            Texture* texture = Texture::Builder()
                .width(w)
                .height(h)
                .levels(0xff)
                .format(channels == 3 ?
                    Texture::InternalFormat::RGB16F : Texture::InternalFormat::RGBA16F)
                .sampler(Texture::Sampler::SAMPLER_2D)
                .build(*gEngine);

            Texture::PixelBufferDescriptor::Callback freeCallback = [](void* buf, size_t, void* data) {
                delete reinterpret_cast<LinearImage*>(data);
                };

            Texture::PixelBufferDescriptor buffer(
                image->getPixelRef(),
                size_t(w * h * channels * sizeof(float)),
                channels == 3 ? Texture::Format::RGB : Texture::Format::RGBA,
                Texture::Type::FLOAT,
                freeCallback,
                image
            );

            texture->setImage(*gEngine, 0, std::move(buffer));

            tex_res->texture = texture;
        }

        if (generateMIPs) {
          tex_res->texture->generateMipmaps(*gEngine);
        }

        if (isNew)
        {
            tex_res->sampler.setMagFilter(TextureSampler::MagFilter::LINEAR);
            tex_res->sampler.setMinFilter(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR);
            tex_res->sampler.setWrapModeS(TextureSampler::WrapMode::REPEAT);
            tex_res->sampler.setWrapModeT(TextureSampler::WrapMode::REPEAT);
        }

        TextureVID tex_vid = GetVID();
        for (MInstanceVID mi_vid : tex_res->assignedMIs)
        {
            VzMIRes* mi_res = gEngineApp->GetMIRes(mi_vid);
            assert(mi_res);
            for (auto& tex_map_kv : mi_res->texMap)
            {
                if (tex_map_kv.second == tex_vid)
                {
                    mi_res->mi->setParameter(tex_map_kv.first.c_str(), tex_res->texture, tex_res->sampler);
                }
            }
        }

        UpdateTimeStamp();
        return true;
    }

    std::string VzTexture::GetImageFileName()
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        if (tex_res == nullptr)
        {
            return "";
        }
        return tex_res->fileName;
    }

    void VzTexture::SetMinFilter(const SamplerMinFilter filter)
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        tex_res->sampler.setMinFilter((TextureSampler::MinFilter)filter);
        UpdateTimeStamp();
    }
    void VzTexture::SetMagFilter(const SamplerMagFilter filter)
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        tex_res->sampler.setMagFilter((TextureSampler::MagFilter)filter);
        UpdateTimeStamp();
    }
    void VzTexture::SetWrapModeS(const SamplerWrapMode mode)
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        tex_res->sampler.setWrapModeS((TextureSampler::WrapMode)mode);
        UpdateTimeStamp();
    }
    void VzTexture::SetWrapModeT(const SamplerWrapMode mode)
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID());
        tex_res->sampler.setWrapModeT((TextureSampler::WrapMode)mode);
        UpdateTimeStamp();
    }

    bool VzTexture::GenerateMIPs()
    {
        VzTextureRes* tex_res = gEngineApp->GetTextureRes(GetVID()); 
        ASYNCCHECK;

        if (tex_res->texture == nullptr) {
            return false;
        }
        tex_res->texture->generateMipmaps(*gEngine);

        UpdateTimeStamp();
        return true;
    }
}
