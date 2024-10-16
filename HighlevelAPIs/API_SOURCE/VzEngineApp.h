#ifndef VZENGINEAPP_H
#define VZENGINEAPP_H
#include "VzComponents.h"

#include "filament/VertexBuffer.h"
#include "filament/IndexBuffer.h"
#include "filament/MorphTargetBuffer.h"

#include "camutils/Manipulator.h"
#include "filament/Box.h"

#include "gltfio/MaterialProvider.h"
#include "gltfio/FilamentAsset.h"
#include "gltfio/ResourceLoader.h"

#include <array>

#include <ft2build.h>
#include FT_FREETYPE_H

using SceneVID = VID;
using RendererVID = VID;
using CamVID = VID;
using ActorVID = VID;
using LightVID = VID;
using GeometryVID = VID;
using MaterialVID = VID;
using TextureVID = VID;
using MInstanceVID = VID;
using AssetVID = VID;
using SkeletonVID = VID;
using BoneVID = VID;
using FontVID = VID;

namespace vzm::backlog
{
    enum class LogLevel
    {
        None,
        Default,
        Warning,
        Error,
    };

    void post(const std::string& input, LogLevel level);
}

// internal helpers
namespace vzm
{
    void getDescendants(const VID vid, std::vector<VID>& decendants);
    void cubeToScene(const VID vidCubeRenderable, const VID vidCube);
}

namespace filament::gltfio {
    struct VzAssetLoader;
    struct VzAssetExpoter;
}

class VzIBL;
class VzCube;
// component contents
namespace vzm
{
    enum class PrimitiveType : uint8_t {
        // don't change the enums values (made to match GL)
        POINTS = 0,    //!< points
        LINES = 1,    //!< lines
        LINE_STRIP = 3,    //!< line strip
        TRIANGLES = 4,    //!< triangles
        TRIANGLE_STRIP = 5     //!< triangle strip
    };

    using namespace filament;
    using namespace filament::gltfio;

    using CameraManipulator = filament::camutils::Manipulator<float>;

    struct GltfIO;

    struct VzPrimitive {
        VertexBuffer* vertices = nullptr;
        IndexBuffer* indices = nullptr;
        Aabb aabb; // object-space bounding box
        UvMap uvmap; // mapping from each glTF UV set to either UV0 or UV1 (8 bytes)
        MorphTargetBuffer* morphTargetBuffer = nullptr;
        uint32_t morphTargetOffset;
        std::vector<int> slotIndices;

        PrimitiveType ptype = PrimitiveType::TRIANGLES;
    };

    struct VzTextFormat {
        FontVID font = INVALID_VID;
        TEXT_ALIGN textAlign = TEXT_ALIGN::LEFT;
        uint32_t kerning = 0;
        uint32_t leading = 0;
    };

    struct VzTypesetter {
        void Measure();
        int32_t MeasureLinesWidth(FontVID font);
        void Typeset();
        int32_t GetLeftBlankWidth(const TEXT_ALIGN textAlign, const int32_t lineWidth, const int32_t width);
        int32_t GetTopBlankHeight(const TEXT_ALIGN textAlign, const int32_t lineHeight, const int32_t height);

        Texture* texture = nullptr;
        std::wstring text;
        std::vector<uint32_t> glyphCodes;
        VzTextFormat textFormat;
        int32_t fixedWidth = 0;
        int32_t fixedHeight = 0;
        std::vector<int32_t> linesWidth;
        int32_t textWidth = 0;
        int32_t textHeight = 0;
    };

    struct VzTextField {
        VzTypesetter typesetter;
        float textColor[4] = { 1.f, 1.f, 1.f, 1.f };
    };

    struct VzSceneRes
    {
    private:
        VzIBL* ibl_ = nullptr;
        VzCube* lightmapCube_ = nullptr; // note current filament supports only one directional light's shadowmap
    public:
        VzSceneRes();
        ~VzSceneRes();

        float iblRotation = 0.f;

        void Destory();
        VzIBL* GetIBL();
        VzIBL* NewIBL();
        VzCube* GetLightmapCube();
    };
    struct VzCameraRes
    {
    private:
        filament::Camera* camera_ = nullptr;
        VzCube* cameraCube_ = nullptr;
        VzCamera::Controller camController_ = VzCamera::Controller(0);
        CameraManipulator* cameraManipulator_;
    public:
        VzCameraRes() = default;
        ~VzCameraRes();

        uint64_t FRAMECOUNT = 0;
        TimeStamp timer;
        float deltaTime = 0;
        float deltaTimeAccumulator = 0;
        size_t width = 0;
        size_t height = 0;
        bool cameraControllerEnabled = false;

        void SetCamera(Camera* camera);
        Camera* GetCamera();
        VzCube* GetCameraCube();
        void NewCameraManipulator(const VzCamera::Controller& camController);
        VzCamera::Controller* GetCameraController();
        CameraManipulator* GetCameraManipulator();
        void UpdateCameraWithCM(float deltaTime);
    };
    struct VzActorRes
    {
    private:
        GeometryVID vidGeo_ = INVALID_VID;
        std::vector<MInstanceVID> vidMIs_;
        std::vector<std::vector<MInstanceVID>> vidMIVariants_;

    public:
        bool isSprite = false;
        bool isBillboard = false;
        bool culling = true;
        bool castShadow = true;
        bool receiveShadow = true;
        uint8_t priority = 0x4;

        void SetGeometry(const GeometryVID vid);
        void SetMIs(const std::vector<MInstanceVID>& vidMIs);
        bool SetMI(const MInstanceVID vid, const int slot);
        void SetMIVariants(const std::vector<std::vector<MInstanceVID>>& vidMIVariants);
        GeometryVID GetGeometryVid();
        MInstanceVID GetMIVid(const int slot);
        std::vector<MInstanceVID>& GetMIVids();
        std::vector<std::vector<MInstanceVID>>& GetMIVariants();

        // for text sprite
        VzTextField textField;
        float anchorU = 0.5f;
        float anchorV = 0.5f;
        float fontHeight = 1.f;

        float spriteWidth = 1.f;
        float spriteHeight = 1.f;

        // for sprite
        VertexBuffer* intrinsicVB = nullptr;
        IndexBuffer* intrinsicIB = nullptr;
        Texture* intrinsicTexture = nullptr;
        std::vector<char> intrinsicCache;

        ~VzActorRes();
    };
    struct VzLightRes
    {
    private:
    public:
        VzLightRes() {};
        ~VzLightRes() {};
    };
    struct VzGeometryRes
    {
    private:
        static std::set<VertexBuffer*> currentVBs_;
        static std::set<IndexBuffer*> currentIBs_;
        static std::set<MorphTargetBuffer*> currentMTBs_;

        std::vector<VzPrimitive> primitives_;
        
        std::vector<char> cacheVB;
        std::vector<char> cacheIB;
        std::vector<char> cacheMTB;
    public:
        bool isSystem = false;
        gltfio::FilamentAsset* assetOwner = nullptr; // has ownership
        filament::Aabb aabb;
        std::vector<float> morphWeights;
        void Set(const std::vector<VzPrimitive>& primitives);
        std::vector<VzPrimitive>* Get();

        ~VzGeometryRes();
    };
    struct VzMaterialRes
    {
        bool isSystem = false;
        gltfio::FilamentAsset* assetOwner = nullptr; // has ownership
        Material* material = nullptr;
        bool isStandardType = true;
        std::unordered_map<std::string, Material::ParameterInfo> allowedParamters;
        ~VzMaterialRes();
    };
    struct VzMIRes
    {
        bool isSystem = false;
        gltfio::FilamentAsset* assetOwner = nullptr; // has ownership
        MaterialVID vidMaterial = INVALID_VID;
        MaterialInstance* mi = nullptr;

        // ... TODO: update... when removing textures 
        std::unordered_map<std::string, TextureVID> texMap; 

        ~VzMIRes();
    };
    struct VzTextureRes
    {
        // read only shader (e.g., SRV)
        bool isSystem = false;
        gltfio::FilamentAsset* assetOwner = nullptr; // has ownership
        Texture* texture = nullptr;
        std::string fileName;
        TextureSampler sampler;
        bool isAsyncLocked = false;

        std::set<MInstanceVID> assignedMIs;

        ~VzTextureRes();
    };
    struct VzFontRes
    {
    private:
        void loadGlyph(const uint32_t glyphCode);
        void renderGlyph(const uint32_t glyphCode);
    public:
        ~VzFontRes();

        bool IsSpace(const uint32_t glyphCode);
        bool IsNewLine(const uint32_t glyphCode);
        int32_t GetLineHeight();
        int32_t GetBearingX(const uint32_t glyphCode);
        int32_t GetBearingY(const uint32_t glyphCode);
        int32_t GetAdvanceX(const uint32_t glyphCode);
        int32_t GetGlyphWidth(const uint32_t glyphCode);
        int32_t GetGlyphHeight(const uint32_t glyphCode);
        const uint8_t* GetGlyphPixels(const uint32_t glyphCode);

        FT_Face ftFace_ = nullptr;
        std::string path_;
        uint32_t size_ = 10;
        uint32_t glyphCode_ = 0x00000000U;
        bool isLoaded_ = false;
        bool isRendered_ = false;
    };

    struct VzAssetRes
    {
        gltfio::FilamentAsset* asset = nullptr;
        std::vector<VID> rootVIDs;
        std::set<VID> assetOwnershipComponents;
        std::vector<SkeletonVID> skeletons;

        // to identify the entity's origin
        std::set<GeometryVID> fromAssetGeometries;
        std::set<MaterialVID> fromAssetMaterials;
        std::set<MInstanceVID> fromAssetMIs;
        std::set<TextureVID> fromAssetTextures;
        std::set<CamVID> fromAssetCameras;
        std::set<LightVID> fromAssetLights;
        std::set<ActorVID> fromAssetRenderableActors;
        std::set<ActorVID> fromAssetNodes;
        std::set<SkeletonVID> fromAssetSketetons;

        VzAsset::Animator animator = VzAsset::Animator(0);

        std::unordered_map<size_t, TextureVID> asyncTextures; // fasset.mTextures
    };
    
    namespace skm {
        struct AnimatorImpl;
    }
    struct VzAniRes
    {
        bool isSystem = false;

        // 'assetOwner' will be deprecated!
        gltfio::FilamentAsset* assetOwner = nullptr; // has ownership

        // move high-level VzAsset::Animator's parameters here
        // TODO

        skm::AnimatorImpl* animator = nullptr;
        ~VzAniRes();
    };

    struct VzSkeletonRes
    {
        std::unordered_map<BoneVID, std::string> bones;
    };

}

namespace vzm
{
    using namespace filament;

    // matc.exe -o compositor.filamat compositor.mat
    struct CompositorQuad
    {
    private:
        VertexBuffer* quadVb_ = nullptr;
        IndexBuffer* quadIb_ = nullptr;
        Material* compositorMaterial_ = nullptr;
        MaterialInstance* compositorMI_ = nullptr;
        Camera* cameraQuad_ = nullptr;
        Scene* sceneQuad_ = nullptr;
        utils::Entity qaudRenderableEntity_;
    public:
        CompositorQuad();
        ~CompositorQuad();

        TextureSampler sampler;
        TextureSampler samplerPoint;

        Camera* GetQaudCamera() { return cameraQuad_; }
        Scene* GetQuadScene() { return sceneQuad_; }
        MaterialInstance* GetMaterial() { return compositorMI_; }
    };


    class VzRenderPath;

    class VzEngineApp
    {
    private:
        std::unordered_map<SceneVID, Scene*> scenes_;
        std::unordered_map<SceneVID, std::unique_ptr<VzSceneRes>> sceneResMap_;
        // note a VzRenderPath involves a filament::view that includes
        // 1. filament::camera and 2. filament::scene
        std::unordered_map<CamVID, SceneVID> camSceneMap_;
        std::unordered_map<CamVID, std::unique_ptr<VzCameraRes>> camResMap_;
        std::unordered_map<ActorVID, SceneVID> actorSceneMap_;
        std::unordered_map<ActorVID, std::unique_ptr<VzActorRes>> actorResMap_; // consider when removing resources...
        std::unordered_map<LightVID, SceneVID> lightSceneMap_;
        std::unordered_map<LightVID, std::unique_ptr<VzLightRes>> lightResMap_;

        std::unordered_map<RendererVID, std::unique_ptr<VzRenderPath>> renderPathMap_;

        // Resources (ownership check!)
        std::unordered_map<GeometryVID, std::unique_ptr<VzGeometryRes>> geometryResMap_;
        std::unordered_map<MaterialVID, std::unique_ptr<VzMaterialRes>> materialResMap_;
        std::unordered_map<MInstanceVID, std::unique_ptr<VzMIRes>> miResMap_;
        std::unordered_map<TextureVID, std::unique_ptr<VzTextureRes>> textureResMap_;
        std::unordered_map<FontVID, std::unique_ptr<VzFontRes>> fontResMap_;

        // GLTF Asset
        std::unordered_map<AssetVID, std::unique_ptr<VzAssetRes>> assetResMap_;
        std::unordered_map<SkeletonVID, std::unique_ptr<VzSkeletonRes>> skeletonResMap_;

        std::unordered_map<VID, std::unique_ptr<VzBaseComp>> vzCompMap_;

        bool removeScene(SceneVID vidScene);

        CompositorQuad* compositor_ = nullptr;

    public:
        // Runtime can create a new entity with this
        VzScene* CreateScene(const std::string& name);
        VzRenderer* CreateRenderPath(const std::string& name);
        VzAsset* CreateAsset(const std::string& name);
        VzSkeleton* CreateSkeleton(const std::string& name, const SkeletonVID vidExist = 0);
        size_t GetVidsByName(const std::string& name, std::vector<VID>& vids);
        VID GetFirstVidByName(const std::string& name);
        std::string GetNameByVid(const VID vid);
        bool HasComponent(const VID vid);
        bool IsRenderable(const ActorVID vid);
        bool IsSceneComponent(VID vid);
        bool IsLight(const LightVID vid);
        Scene* GetScene(const SceneVID vid);
        Scene* GetFirstSceneByName(const std::string& name);
        std::unordered_map<SceneVID, Scene*>* GetScenes();

        VzSceneRes* GetSceneRes(const SceneVID vid);
        VzRenderPath* GetRenderPath(const RendererVID vid);
        VzCameraRes* GetCameraRes(const CamVID vid);
        VzActorRes* GetActorRes(const ActorVID vid);
        VzLightRes* GetLightRes(const LightVID vid);
        VzAssetRes* GetAssetRes(const AssetVID vid);
        std::unordered_map<AssetVID, std::unique_ptr<VzAssetRes>>* GetAssetResMap() {
            return &assetResMap_;
        }
        VzSkeletonRes* GetSkeletonRes(const SkeletonVID vid);
        AssetVID GetAssetOwner(VID vid);

        size_t GetCameraVids(std::vector<CamVID>& camVids);
        size_t GetActorVids(std::vector<ActorVID>& actorVids);
        size_t GetLightVids(std::vector<LightVID>& lightVids);
        size_t GetRenderPathVids(std::vector<RendererVID>& renderPathVids);
        VzRenderPath* GetFirstRenderPathByName(const std::string& name);
        SceneVID GetSceneVidBelongTo(const VID vid);
        size_t GetSceneCompChildren(const SceneVID vidScene, std::vector<VID>& vidChildren);

        CompositorQuad* GetCompositorQuad()
        {
            return compositor_;
        }

        bool AppendSceneEntityToParent(const VID vidSrc, const VID vidDst);

        VzSceneComp* CreateSceneComponent(const SCENE_COMPONENT_TYPE compType,
            const std::string& name, const VID vidExist = 0);
        VzActor* CreateTestActor(const std::string& modelName = "MONKEY_SUZANNE_DATA");
        VzGeometry* CreateGeometry(const std::string& name,
            const std::vector<VzPrimitive>& primitives,
            const filament::gltfio::FilamentAsset* assetOwner = nullptr,
            const bool isSystem = false);
        VzMaterial* CreateMaterial(const std::string& name,
            const Material* material = nullptr,
            const filament::gltfio::FilamentAsset* assetOwner = nullptr,
            const bool isSystem = false);
        VzMI* CreateMaterialInstance(const std::string& name,
            const MaterialVID vidMaterial,
            const MaterialInstance* mi = nullptr,
            const filament::gltfio::FilamentAsset* assetOwner = nullptr,
            const bool isSystem = false);
        VzTexture* CreateTexture(const std::string& name,
            const Texture* texture = nullptr,
            const filament::gltfio::FilamentAsset* assetOwner = nullptr,
            const bool isSystem = false);
        VzFont* CreateFont(const std::string& name);

        void BuildRenderable(const ActorVID vid);

        VzGeometryRes* GetGeometryRes(const GeometryVID vidGeo);
        VzMaterialRes* GetMaterialRes(const MaterialVID vidMaterial);
        MaterialVID FindMaterialVID(const filament::Material* mat);

        VzMIRes* GetMIRes(const MInstanceVID vidMI);
        MInstanceVID FindMaterialInstanceVID(const filament::MaterialInstance* mi);

        VzTextureRes* GetTextureRes(const TextureVID vidTex);
        TextureVID FindTextureVID(const filament::Texture* texture);

        VzFontRes* GetFontRes(const FontVID vidFont);

        template <typename VZCOMP>
        VZCOMP* GetVzComponent(const VID vid)
        {
            auto it = vzCompMap_.find(vid);
            if (it == vzCompMap_.end())
            {
                return nullptr;
            }
            return (VZCOMP*)it->second.get();
        }
        size_t GetVzComponentsByType(const std::string& type, std::vector<VzBaseComp*>& components)
        {
            components.clear();
            for (auto& it : vzCompMap_)
            {
                if (it.second->GetType() == type)
                {
                    components.push_back(it.second.get());
                }
            }
            return components.size();
        }

        size_t LoadMeshFile(const std::string& filename, std::vector<VzActor*>& actors);

        gltfio::VzAssetLoader* GetGltfAssetLoader();
        gltfio::VzAssetExpoter* GetGltfAssetExpoter();
        gltfio::ResourceLoader* GetGltfResourceLoader();

        template <typename UM> void destroyTarget(UM& umap)
        {
            std::vector<VID> vids;
            vids.reserve(umap.size());
            for (auto it = umap.begin(); it != umap.end(); it++)
            {
                vids.push_back(it->first);
            }
            for (VID vid : vids)
            {
                RemoveComponent(vid);
            }
        }
        bool RemoveComponent(const VID vid, const bool forceToRemove = false);

        void CancelAyncLoad();
        void Initialize();
        void Destroy();

        AssetVID activeAsyncAsset = INVALID_VID;

        FT_Library ftLibrary = nullptr;
    };
}

#define COMP_NAME(COMP, ENTITY, FAILRET) auto& COMP = VzNameCompManager::Get(); Entity ENTITY = Entity::import(GetVID()); if (ENTITY.isNull()) return FAILRET;
#define COMP_TRANSFORM(COMP, ENTITY, INS, FAILRET)  auto & COMP = gEngine->getTransformManager(); Entity ENTITY = Entity::import(GetVID()); if (ENTITY.isNull()) return FAILRET; auto INS = COMP.getInstance(ENTITY);
#define COMP_RENDERPATH(RENDERPATH, FAILRET)  VzRenderPath* RENDERPATH = gEngineApp->GetRenderPath(GetVID()); if (RENDERPATH == nullptr) return FAILRET;
#define COMP_LIGHT(COMP, ENTITY, INS, FAILRET)  auto & COMP = gEngine->getLightManager(); Entity ENTITY = Entity::import(GetVID()); if (ENTITY.isNull()) return FAILRET; auto INS = COMP.getInstance(ENTITY);
#define COMP_ACTOR(COMP, ENTITY, INS, FAILRET)  auto & COMP = gEngine->getRenderableManager(); Entity ENTITY = Entity::import(GetVID()); if (ENTITY.isNull()) return FAILRET; auto INS = COMP.getInstance(ENTITY);
#define COMP_MI(COMP, RES, FAILRET) VzMIRes* RES = gEngineApp->GetMIRes(GetVID()); if (RES == nullptr) return FAILRET; MaterialInstance* COMP = RES->mi; if (COMP == nullptr) return FAILRET;
#define COMP_MAT(COMP, RES, FAILRET) VzMaterialRes* RES = gEngineApp->GetMaterialRes(GetVID()); if (RES == nullptr) return FAILRET; Material* COMP = RES->material; if (COMP == nullptr) return FAILRET;
#define COMP_CAMERA(COMP, ENTITY, FAILRET) Entity ENTITY = Entity::import(GetVID()); Camera* COMP = gEngine->getCameraComponent(ENTITY); if (COMP == nullptr) return FAILRET;
#define COMP_ASSET(COMP, FAILRET)  VzAssetRes* COMP = gEngineApp->GetAssetRes(GetVID()); assert(COMP->asset->getAssetInstanceCount() == 1); // later... for multi-instance cases

using namespace vzm;
#endif
