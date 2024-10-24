#pragma once
#include "../VizComponentAPIs.h"

namespace vzm
{
    struct API_EXPORT VzSkeleton : VzBaseComp
    {
        VzSkeleton(const VID vid, const std::string& originFrom) :
            VzBaseComp(vid, originFrom, "VzSkeleton") {}

        using BoneVID = VID;

        //! Returns a list of all bones in the skeleton
        std::vector<BoneVID> GetBones();

        //! Pass the identity matrix into all bone nodes, useful for returning to the T pose
        void ResetBoneMatrices();

        //! Computes root-to-node transforms for all bone nodes, then passes the results
        void UpdateBoneMatrices();
    };
}
