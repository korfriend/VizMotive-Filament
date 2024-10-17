#pragma once
#include <map>
#include <string>
#include <vector>

#include <cgltf.h>
#include <math/mat4.h>
#include <math/quat.h>
#include <math/scalar.h>
#include <math/vec3.h>
#include <math/vec4.h>

#include <tsl/robin_set.h>

#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>

#include "../../libs/gltfio/src/FTrsTransformManager.h"

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

    struct Skin
    {
        // The list of entities whose transform components define the joints of the skin.
        utils::FixedCapacityVector<utils::Entity> joints;

        // The set of all entities that are influenced by this skin.
        // This is initially derived from the glTF, but users can dynamically add or remove targets.
        tsl::robin_set<utils::Entity, utils::Entity::Hasher> targets;
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
        std::string name;
        vector<Sampler> samplers;
        vector<Channel> channels;
    };

    struct AnimatorImpl
    {
        vector<Animation> animations;
        BoneVector boneMatrices;
        //FFilamentAsset const* asset = nullptr;
        //FFilamentInstance* instance = nullptr;
        AnimatorVID animatorVID = INVALID_VID; // TODO: SkeletonVID?
        std::vector<Skin> skins;
        Entity rootEntity;
        RenderableManager* renderableManager;
        TransformManager* transformManager;
        TrsTransformManager* trsTransformManager;
        vector<float> weights;
        FixedCapacityVector<mat4f> crossFade;
        //void addChannels(const FixedCapacityVector<Entity>& nodeMap, const cgltf_animation& srcAnim,
        //    Animation& dst);
        void addChannels(const FixedCapacityVector<Entity>& nodeMap, const cgltf_node* nodes, const cgltf_animation& srcAnim, Animation& dst);
        void applyAnimation(const Channel& channel, float t, size_t prevIndex, size_t nextIndex);
        void stashCrossFade();
        void applyCrossFade(float alpha);

        //void resetBoneMatrices(FFilamentInstance* instance);
        //void updateBoneMatrices(FFilamentInstance* instance);
        void resetBoneMatrices(std::vector<Skin>& skins);
        void updateBoneMatrices(std::vector<Skin>& skins);
    };

    void createSampler(const cgltf_animation_sampler& src, Sampler& dst);
    void setTransformType(const cgltf_animation_channel& src, Channel& dst);
    bool validateAnimation(const cgltf_animation& anim);
}
