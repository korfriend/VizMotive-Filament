#include "VzSkeleton.h"
#include "../VzEngineApp.h"
#include "../FIncludes.h"

#include "../backend/VzAnimator.h"

extern Engine* gEngine;
extern vzm::VzEngineApp* gEngineApp;

namespace vzm
{
#define COMP_SKELETON(COMP, FAILRET)                            \
    VzSkeletonRes* COMP = gEngineApp->GetSkeletonRes(GetVID()); \
    if (COMP == nullptr)                                        \
        return FAILRET;

    std::vector<BoneVID> VzSkeleton::GetBones()
    {
        COMP_SKELETON(skel_res, {});
        auto result = std::vector<BoneVID>();
        for (auto& joint : skel_res->skeleton->joints)
        {
            result.push_back(joint.getId());
        }
        return result;
    }
    void VzSkeleton::ResetBoneMatrices()
    {
        COMP_SKELETON(skel_res, );
        size_t njoints = skel_res->skeleton->joints.size();
        auto& boneMatrices = skel_res->boneMatrices;
        boneMatrices.resize(njoints);
        auto& rcm = gEngine->getRenderableManager();
        for (const auto& entity : skel_res->skeleton->targets)
        {
            auto renderable = rcm.getInstance(entity);
            if (renderable)
            {
                for (size_t boneIndex = 0; boneIndex < njoints; ++boneIndex)
                {
                    boneMatrices[boneIndex] = mat4f();
                }
                rcm.setBones(renderable, boneMatrices.data(), boneMatrices.size());
            }
        }
    }
    void VzSkeleton::UpdateBoneMatrices()
    {
        COMP_SKELETON(skel_res, );
        size_t njoints = skel_res->skeleton->joints.size();
        auto& boneMatrices = skel_res->boneMatrices;
        boneMatrices.resize(njoints);
        auto& rcm = gEngine->getRenderableManager();
        auto& tcm = gEngine->getTransformManager();
        for (Entity entity : skel_res->skeleton->targets)
        {
            auto renderable = rcm.getInstance(entity);
            if (!renderable)
            {
                continue;
            }
            mat4 inverseGlobalTransform;
            auto xformable = tcm.getInstance(entity);
            if (xformable)
            {
                inverseGlobalTransform = inverse(tcm.getWorldTransformAccurate(xformable));
            }
            for (size_t boneIndex = 0; boneIndex < njoints; ++boneIndex)
            {
                const auto& joint = skel_res->skeleton->joints[boneIndex];
                const mat4f& inverseBindMatrix = skel_res->skeleton->inverseBindMatrices[boneIndex];
                TransformManager::Instance jointInstance = tcm.getInstance(joint);
                mat4 globalJointTransform = tcm.getWorldTransformAccurate(jointInstance);
                boneMatrices[boneIndex] = mat4f{ inverseGlobalTransform * globalJointTransform } * inverseBindMatrix;
            }
            rcm.setBones(renderable, boneMatrices.data(), boneMatrices.size());
        }
    }
}
