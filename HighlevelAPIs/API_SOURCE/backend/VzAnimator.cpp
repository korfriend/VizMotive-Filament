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
#if 0
    void AnimatorImpl::applyAnimation(const Channel& channel, float t, size_t prevIndex, size_t nextIndex)
    {
        const Sampler* sampler = channel.sourceData;
        const TimeValues& times = sampler->times;
        TrsTransformManager::Instance trsNode = trsTransformManager->getInstance(channel.targetEntity);
        TransformManager::Instance node = transformManager->getInstance(channel.targetEntity);

        switch (channel.transformType)
        {
        case Channel::SCALE:
        {
            float3 scale;
            const float3* srcVec3 = (const float3*)sampler->values.data();
            if (sampler->interpolation == Sampler::CUBIC)
            {
                float3 vert0 = srcVec3[prevIndex * 3 + 1];
                float3 tang0 = srcVec3[prevIndex * 3 + 2];
                float3 tang1 = srcVec3[nextIndex * 3];
                float3 vert1 = srcVec3[nextIndex * 3 + 1];
                scale = cubicSpline(vert0, tang0, vert1, tang1, t);
            }
            else
            {
                scale = ((1 - t) * srcVec3[prevIndex]) + (t * srcVec3[nextIndex]);
            }
            trsTransformManager->setScale(trsNode, scale);
            break;
        }

        case Channel::TRANSLATION:
        {
            float3 translation;
            const float3* srcVec3 = (const float3*)sampler->values.data();
            if (sampler->interpolation == Sampler::CUBIC)
            {
                float3 vert0 = srcVec3[prevIndex * 3 + 1];
                float3 tang0 = srcVec3[prevIndex * 3 + 2];
                float3 tang1 = srcVec3[nextIndex * 3];
                float3 vert1 = srcVec3[nextIndex * 3 + 1];
                translation = cubicSpline(vert0, tang0, vert1, tang1, t);
            }
            else
            {
                translation = ((1 - t) * srcVec3[prevIndex]) + (t * srcVec3[nextIndex]);
            }
            trsTransformManager->setTranslation(trsNode, translation);
            break;
        }

        case Channel::ROTATION:
        {
            quatf rotation;
            const quatf* srcQuat = (const quatf*)sampler->values.data();
            if (sampler->interpolation == Sampler::CUBIC)
            {
                quatf vert0 = srcQuat[prevIndex * 3 + 1];
                quatf tang0 = srcQuat[prevIndex * 3 + 2];
                quatf tang1 = srcQuat[nextIndex * 3];
                quatf vert1 = srcQuat[nextIndex * 3 + 1];
                rotation = normalize(cubicSpline(vert0, tang0, vert1, tang1, t));
            }
            else
            {
                rotation = slerp(srcQuat[prevIndex], srcQuat[nextIndex], t);
            }
            trsTransformManager->setRotation(trsNode, rotation);
            break;
        }

        case Channel::WEIGHTS:
        {
            const float* const samplerValues = sampler->values.data();
            assert(sampler->values.size() % times.size() == 0);
            const int valuesPerKeyframe = sampler->values.size() / times.size();

            if (sampler->interpolation == Sampler::CUBIC)
            {
                assert(valuesPerKeyframe % 3 == 0);
                const int numMorphTargets = valuesPerKeyframe / 3;
                const float* const inTangents = samplerValues;
                const float* const splineVerts = samplerValues + numMorphTargets;
                const float* const outTangents = samplerValues + numMorphTargets * 2;

                weights.resize(numMorphTargets);
                for (int comp = 0; comp < numMorphTargets; ++comp)
                {
                    float vert0 = splineVerts[comp + prevIndex * valuesPerKeyframe];
                    float tang0 = outTangents[comp + prevIndex * valuesPerKeyframe];
                    float tang1 = inTangents[comp + nextIndex * valuesPerKeyframe];
                    float vert1 = splineVerts[comp + nextIndex * valuesPerKeyframe];
                    weights[comp] = cubicSpline(vert0, tang0, vert1, tang1, t);
                }
            }
            else
            {
                weights.resize(valuesPerKeyframe);
                for (int comp = 0; comp < valuesPerKeyframe; ++comp)
                {
                    float previous = samplerValues[comp + prevIndex * valuesPerKeyframe];
                    float current = samplerValues[comp + nextIndex * valuesPerKeyframe];
                    weights[comp] = (1 - t) * previous + t * current;
                }
            }

            auto ci = renderableManager->getInstance(channel.targetEntity);
            renderableManager->setMorphWeights(ci, weights.data(), weights.size());
            return;
        }
        }

        transformManager->setTransform(node, trsTransformManager->getTransform(trsNode));
    }

    void AnimatorImpl::stashCrossFade()
    {
        using Instance = TransformManager::Instance;
        auto& tm = *this->transformManager;
        auto& stash = this->crossFade;

        // Count the total number of transformable nodes to preallocate the stash memory.
        // We considered caching this count, but the cache would need to be invalidated when entities
        // are added into the hierarchy.
        auto recursiveCount = [&tm](Instance node, size_t count, auto& fn) -> size_t {
            ++count;
            for (auto iter = tm.getChildrenBegin(node); iter != tm.getChildrenEnd(node); ++iter)
            {
                count = fn(*iter, count, fn);
            }
            return count;
        };

        auto recursiveStash = [&tm, &stash](Instance node, size_t index, auto& fn) -> size_t {
            stash[index++] = tm.getTransform(node);
            for (auto iter = tm.getChildrenBegin(node); iter != tm.getChildrenEnd(node); ++iter)
            {
                index = fn(*iter, index, fn);
            }
            return index;
        };

        const Instance root = tm.getInstance(rootEntity);
        const size_t count = recursiveCount(root, 0, recursiveCount);
        crossFade.reserve(count);
        crossFade.resize(count);
        recursiveStash(root, 0, recursiveStash);
    }

    void AnimatorImpl::applyCrossFade(float alpha)
    {
        using Instance = TransformManager::Instance;
        auto& tm = *this->transformManager;
        auto& stash = this->crossFade;
        auto recursiveFn = [&tm, &stash, alpha](Instance node, size_t index, auto& fn) -> size_t {
            float3 scale0, scale1;
            quatf rotation0, rotation1;
            float3 translation0, translation1;
            decomposeMatrix(stash[index++], &translation1, &rotation1, &scale1);
            decomposeMatrix(tm.getTransform(node), &translation0, &rotation0, &scale0);
            const float3 scale = mix(scale0, scale1, alpha);
            const quatf rotation = slerp(rotation0, rotation1, alpha);
            const float3 translation = mix(translation0, translation1, alpha);
            tm.setTransform(node, composeMatrix(translation, rotation, scale));
            for (auto iter = tm.getChildrenBegin(node); iter != tm.getChildrenEnd(node); ++iter)
            {
                index = fn(*iter, index, fn);
            }
            return index;
        };
        const Instance root = tm.getInstance(rootEntity);
        recursiveFn(root, 0, recursiveFn);
    }
#endif
} // namespace vzm::skm
