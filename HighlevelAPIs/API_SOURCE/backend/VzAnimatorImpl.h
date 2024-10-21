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

    struct Skeleton
    {
        // The list of entities whose transform components define the joints of the skin.
        utils::FixedCapacityVector<utils::Entity> joints;

        // The set of all entities that are influenced by this skin.
        // This is initially derived from the glTF, but users can dynamically add or remove targets.
        tsl::robin_set<utils::Entity, utils::Entity::Hasher> targets;

        utils::CString name;
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
        std::string name;
        vector<Sampler> samplers;   // bones
        vector<Channel> channels;   // bones...156?
        vector<float> weights;

        Entity skeletonEntity;
    };

    Animation skm_walk; // skm has 10 joints and 20 key frames (only TR channels)

    struct AnimatorImpl
    {
        std::map<utils::Entity, Animation> mapAnimations;
        std::map<utils::Entity, Skeleton> mapSkeletons;

        //BoneVector boneMatrices;  // ==> renderable, VzActorRes

        //FFilamentAsset const* asset = nullptr;
        //FFilamentInstance* instance = nullptr;
        //AnimatorVID animatorVID = INVALID_VID; // TODO: SkeletonVID?
        //std::vector<Skeleton> skins; ==> mapSkeletons
        //Entity rootEntity;
        //vector<float> weights;
        FixedCapacityVector<mat4f> crossFade;



        //void addChannels(const FixedCapacityVector<Entity>& nodeMap, const cgltf_animation& srcAnim,
        //    Animation& dst);
        void addChannels(const FixedCapacityVector<Entity>& nodeMap, const cgltf_node* nodes, const cgltf_animation& srcAnim, Animation& dst);
        void applyAnimation(const Channel& channel, float t, size_t prevIndex, size_t nextIndex);
        void stashCrossFade();
        void applyCrossFade(float alpha);

        //void resetBoneMatrices(FFilamentInstance* instance);
        //void updateBoneMatrices(FFilamentInstance* instance);
        void resetBoneMatrices(std::vector<Skeleton>& skins);
        void updateBoneMatrices(std::vector<Skeleton>& skins);
    };

    void createSampler(const cgltf_animation_sampler& src, Sampler& dst);
    void setTransformType(const cgltf_animation_channel& src, Channel& dst);
    bool validateAnimation(const cgltf_animation& anim);
}
