#pragma once
#include <string>
#include <vector>

#include <cgltf.h>
#include <math/mat4.h>

#include <tsl/robin_set.h>

using namespace filament;
using namespace filament::math;
using namespace utils;
using namespace filament::gltfio;

namespace vzm::skm
{
    using namespace std;

    using TimeValues = map<float, size_t>;
    using SourceValues = vector<float>;
    using BoneVector = vector<mat4f>;

    struct Skeleton
    {
        // The list of entities whose transform components define the joints of the skin.
        utils::FixedCapacityVector<utils::Entity> joints;

        // The set of all entities that are influenced by this skin.
        // This is initially derived from the glTF, but users can dynamically add or remove targets.
        tsl::robin_set<utils::Entity, utils::Entity::Hasher> targets;

        utils::FixedCapacityVector<math::mat4f> inverseBindMatrices;
    };

    struct Sampler
    {
        TimeValues times;
        SourceValues values;
        enum
        {
            LINEAR,
            STEP,
            CUBIC
        } interpolation;
    };

    struct Channel
    {
        const Sampler* sourceData;
        Entity targetEntity;
        enum
        {
            TRANSLATION,
            ROTATION,
            SCALE,
            WEIGHTS
        } transformType;
    };

    struct Animation
    {
        float duration;
        vector<Sampler> samplers;
        vector<Channel> channels;
        vector<float> weights;

        Entity skeletonEntity;
    };

    void createSampler(const cgltf_animation_sampler& src, Sampler& dst);
    void setTransformType(const cgltf_animation_channel& src, Channel& dst);
    bool validateAnimation(const cgltf_animation& anim);
}
