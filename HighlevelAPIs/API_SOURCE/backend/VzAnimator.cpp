#include "../VzEngineApp.h"
#include "VzAnimator.h"

#include "../../libs/gltfio/src/FFilamentAsset.h" // FFilamentAsset

extern vzm::VzEngineApp* gEngineApp;

namespace vzm::skm
{
    void createSampler(const cgltf_animation_sampler& src, Sampler& dst)
    {
        // Copy the time values into a red-black tree.
        const cgltf_accessor* timelineAccessor = src.input;
        const uint8_t* timelineBlob = nullptr;
        const float* timelineFloats = nullptr;
        if (timelineAccessor->buffer_view->has_meshopt_compression)
        {
            timelineBlob = (const uint8_t*)timelineAccessor->buffer_view->data;
            timelineFloats = (const float*)(timelineBlob + timelineAccessor->offset);
        }
        else
        {
            timelineBlob = (const uint8_t*)timelineAccessor->buffer_view->buffer->data;
            timelineFloats = (const float*)(timelineBlob + timelineAccessor->offset +
                                            timelineAccessor->buffer_view->offset);
        }
        for (size_t i = 0, len = timelineAccessor->count; i < len; ++i)
        {
            dst.times[timelineFloats[i]] = i;
        }

        // Convert source data to float.
        const cgltf_accessor* valuesAccessor = src.output;
        switch (valuesAccessor->type)
        {
        case cgltf_type_scalar:
            dst.values.resize(valuesAccessor->count);
            cgltf_accessor_unpack_floats(src.output, &dst.values[0], valuesAccessor->count);
            break;
        case cgltf_type_vec3:
            dst.values.resize(valuesAccessor->count * 3);
            cgltf_accessor_unpack_floats(src.output, &dst.values[0], valuesAccessor->count * 3);
            break;
        case cgltf_type_vec4:
            dst.values.resize(valuesAccessor->count * 4);
            cgltf_accessor_unpack_floats(src.output, &dst.values[0], valuesAccessor->count * 4);
            break;
        default:
            backlog::post("Unknown animation type.", backlog::LogLevel::Warning);
            return;
        }

        switch (src.interpolation)
        {
        case cgltf_interpolation_type_linear:
            dst.interpolation = Sampler::LINEAR;
            break;
        case cgltf_interpolation_type_step:
            dst.interpolation = Sampler::STEP;
            break;
        case cgltf_interpolation_type_cubic_spline:
            dst.interpolation = Sampler::CUBIC;
            break;
        case cgltf_interpolation_type_max_enum:
            break;
        }
    }

    void setTransformType(const cgltf_animation_channel& src, Channel& dst)
    {
        switch (src.target_path)
        {
        case cgltf_animation_path_type_translation:
            dst.transformType = Channel::TRANSLATION;
            break;
        case cgltf_animation_path_type_rotation:
            dst.transformType = Channel::ROTATION;
            break;
        case cgltf_animation_path_type_scale:
            dst.transformType = Channel::SCALE;
            break;
        case cgltf_animation_path_type_weights:
            dst.transformType = Channel::WEIGHTS;
            break;
        case cgltf_animation_path_type_max_enum:
        case cgltf_animation_path_type_invalid:
            backlog::post("Unsupported channel path.", backlog::LogLevel::Warning);
            break;
        }
    }

    bool validateAnimation(const cgltf_animation& anim)
    {
        for (cgltf_size j = 0; j < anim.channels_count; ++j)
        {
            const cgltf_animation_channel& channel = anim.channels[j];
            const cgltf_animation_sampler* sampler = channel.sampler;
            if (!channel.target_node)
            {
                continue;
            }
            if (!channel.sampler)
            {
                return false;
            }
            cgltf_size components = 1;
            if (channel.target_path == cgltf_animation_path_type_weights)
            {
                if (!channel.target_node->mesh || !channel.target_node->mesh->primitives_count)
                {
                    return false;
                }
                components = channel.target_node->mesh->primitives[0].targets_count;
            }
            cgltf_size values = sampler->interpolation == cgltf_interpolation_type_cubic_spline ? 3 : 1;
            if (sampler->input->count * components * values != sampler->output->count)
            {
                return false;
            }
        }
        return true;
    }
} // namespace vzm::skm
