#include "VizEngineAPIs.h" 
#include "VzEngineApp.h"

#include "PreDefs.h"

#if FILAMENT_DISABLE_MATOPT
#define OPTIMIZE_MATERIALS false
#else
#define OPTIMIZE_MATERIALS true
#endif

#define NOGDI
#include <backend/platforms/VulkanPlatform.h>

#include "VzRenderPath.h"

#include <iostream>
#include <fstream>
#include <memory>

//#include "FIncludes.h"
#include "backend/VzAnimatorImpl.h"
#include "backend/VzAssetLoader.h"
#include "backend/VzAssetExporter.h"
using namespace vzm;

//////////////////////////////
// filament math
#include <math/mathfwd.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat3.h>
#include <math/mat4.h>
#include <math/norm.h>
#include <math/quat.h>
inline float3 transformCoord(const mat4f& m, const float3& p)
{
    float4 _p(p, 1.f);
    _p = m * _p;
    return float3(_p.x / _p.w, _p.y / _p.w, _p.z / _p.w);
}
inline float3 transformVec(const mat3f& m, const float3& v)
{
    return m * v;
}
//////////////////////////////

#include "VzNameComponents.hpp"

class FilamentAppVulkanPlatform : public VulkanPlatform {
public:
    FilamentAppVulkanPlatform(char const* gpuHintCstr) {
        utils::CString gpuHint{ gpuHintCstr };
        if (gpuHint.empty()) {
            return;
        }
        VulkanPlatform::Customization::GPUPreference pref;
        // Check to see if it is an integer, if so turn it into an index.
        if (std::all_of(gpuHint.begin(), gpuHint.end(), ::isdigit)) {
            char* p_end{};
            pref.index = static_cast<int8_t>(std::strtol(gpuHint.c_str(), &p_end, 10));
        }
        else {
            pref.deviceName = gpuHint;
        }
        mCustomization = {
            .gpu = pref
        };
    }

    virtual VulkanPlatform::Customization getCustomization() const noexcept override {
        return mCustomization;
    }

private:
    VulkanPlatform::Customization mCustomization;
};

static bool gIsDisplay = true;
auto failRet = [](const std::string& err_str, const bool _warn = false)
    {
        if (gIsDisplay)
        {
            vzm::backlog::post(err_str, _warn ? vzm::backlog::LogLevel::Warning : vzm::backlog::LogLevel::Error);
        }
        return false;
    };


VzConfig gConfig;
Engine::Config gEngineConfig = {};
filament::backend::VulkanPlatform* gVulkanPlatform = nullptr;
filament::SwapChain* gDummySwapChain = nullptr;
filament::Material* gMaterialTransparent = nullptr; // do not release
Engine* gEngine = nullptr;
VzEngineApp* gEngineApp = nullptr;

#define CHECK_API_VALIDITY(RET) if (gEngineApp == nullptr) { backlog::post("High-level API is not initialized!!", backlog::LogLevel::Error); return RET; }

enum MaterialSource {
    JITSHADER,
    UBERSHADER,
};

gltfio::MaterialProvider* gMaterialProvider = nullptr;
std::vector<std::string> gMProp = {
            "baseColor",              //!< float4, all shading models
            "roughness",               //!< float,  lit shading models only
            "metallic",                //!< float,  all shading models, except unlit and cloth
            "reflectance",             //!< float,  all shading models, except unlit and cloth
            "ambientOcclusion",       //!< float,  lit shading models only, except subsurface and cloth
            "clearCoat",              //!< float,  lit shading models only, except subsurface and cloth
            "clearCoatRoughness",    //!< float,  lit shading models only, except subsurface and cloth
            "clearCoatNormal",       //!< float,  lit shading models only, except subsurface and cloth
            "anisotropy",              //!< float,  lit shading models only, except subsurface and cloth
            "anisotropyDirection",    //!< float3, lit shading models only, except subsurface and cloth
            "thickness",               //!< float,  subsurface shading model only
            "subsurfacePower",        //!< float,  subsurface shading model only
            "subsurfaceColor",        //!< float3, subsurface and cloth shading models only
            "sheenColor",             //!< float3, lit shading models only, except subsurface
            "sheenRoughness",         //!< float3, lit shading models only, except subsurface and cloth
            "specularColor",          //!< float3, specular-glossiness shading model only
            "glossiness",              //!< float,  specular-glossiness shading model only
            "emissive",                //!< float4, all shading models
            "normal",                  //!< float3, all shading models only, except unlit
            "postLightingColor",     //!< float4, all shading models
            "postLightingMixFactor",//!< float, all shading models
            "clipSpaceTransform",    //!< mat4,   vertex shader only
            "absorption",              //!< float3, how much light is absorbed by the material
            "transmission",            //!< float,  how much light is refracted through the material
            "ior",                     //!< float,  material's index of refraction
            "microThickness",         //!< float, thickness of the thin layer
            "bentNormal",             //!< float3, all shading models only, except unlit
            "specularFactor",         //!< float, lit shading models only, except subsurface and cloth
            "specularColorFactor",   //!< float3, lit shading models only, except subsurface and cloth
};

namespace vzm
{
    using Entity = utils::Entity;
    struct SafeReleaseChecker
    {
        SafeReleaseChecker() {};
        bool destroyed = false;
        ~SafeReleaseChecker()
        {
            if (!destroyed)
            {
                backlog::post("MUST CALL DeinitEngineLib before finishing the application!", backlog::LogLevel::Error);
                DeinitEngineLib();
            }
            backlog::post("Safely finished ^^", backlog::LogLevel::Default);
        };
    };
    std::unique_ptr<SafeReleaseChecker> safeReleaseChecker;
    std::vector<MaterialVID> vzmMaterials;
    std::vector<GeometryVID> vzmGeometries;

    extern "C" API_EXPORT bool IsEngineAvailable()
    {
        return gEngine != nullptr;
    }

    VZRESULT InitEngineLib(const vzm::ParamMap<std::string>& arguments)
    {
        if (gEngine)
        {
            backlog::post("Already initialized!", backlog::LogLevel::Error);
            return VZ_WARNNING;
        }
        assert(gEngineApp == nullptr);
        gEngineApp = new VzEngineApp();

        auto& em = utils::EntityManager::get();
        backlog::post("Entity Manager is activated (# of entities : " + std::to_string(em.getEntityCount()) + ")", 
            backlog::LogLevel::Default);

        gEngineConfig.stereoscopicEyeCount = gConfig.stereoscopicEyeCount;
        gEngineConfig.stereoscopicType = Engine::StereoscopicType::INSTANCED;
        gEngineConfig.disableHandleUseAfterFreeCheck = true;  // TODO: remove this line after fixing the issue
        // to do : gConfig and gEngineConfig
        // using vzm::ParamMap<std::string>& argument
        //gConfig.headless = true;

        gConfig.title = "hellopbr";
        //gConfig.iblDirectory = FilamentApp::getRootAssetsPath() + IBL_FOLDER;
        auto api = arguments.GetParam("api", std::string("opengl"));
        if (api == "opengl")
        {
            gConfig.backend = filament::Engine::Backend::OPENGL;
        }
        else if (api == "vulkan")
        {
            gConfig.backend = filament::Engine::Backend::VULKAN;
            gConfig.vulkanGPUHint = arguments.GetParam("vulkan-gpu-hint", std::string("0"));
        }
        else
        {
            backlog::post("Unrecognized backend. Must be 'opengl'|'vulkan'.", backlog::LogLevel::Error);
            return VZ_FAIL;
        }

        if (gConfig.backend == filament::Engine::Backend::VULKAN)
        {
            gVulkanPlatform = new FilamentAppVulkanPlatform(gConfig.vulkanGPUHint.c_str());
            gEngine = Engine::Builder()
                .backend(gConfig.backend)
                .platform(gVulkanPlatform)
                .featureLevel(filament::backend::FeatureLevel::FEATURE_LEVEL_3)
                .config(&gEngineConfig)
                .build();
        }
        else
        {
            gEngine = Engine::Builder()
                .backend(gConfig.backend)
                .featureLevel(filament::backend::FeatureLevel::FEATURE_LEVEL_3)
                .config(&gEngineConfig)
                .build();
        }

        gEngine->enableAccurateTranslations();

        // this is to avoid the issue of filament safe-resource logic for Vulkan,
        // which assumes that there is at least one swapchain.
        gDummySwapChain = gEngine->createSwapChain((uint32_t)1, (uint32_t)1);

        if (safeReleaseChecker == nullptr)
        {
            safeReleaseChecker = std::make_unique<SafeReleaseChecker>();
        }
        else
        {
            safeReleaseChecker->destroyed = false;
        }

        // optional... test later
        // createUbershaderProvider(gEngine, UBERARCHIVE_DEFAULT_DATA, UBERARCHIVE_DEFAULT_SIZE);
        gMaterialProvider = createJitShaderProvider(gEngine, OPTIMIZE_MATERIALS);

        // default resources
        {
            Material* material = Material::Builder()
                .package(FILAMENTAPP_DEPTHVISUALIZER_DATA, FILAMENTAPP_DEPTHVISUALIZER_SIZE)
                .build(*gEngine);
            vzmMaterials.push_back(gEngineApp->CreateMaterial("_DEFAULT_DEPTH_MATERIAL", material, nullptr, true)->GetVID());

            material = Material::Builder()
                //.package(RESOURCES_SANDBOXUNLIT_DATA, RESOURCES_SANDBOXUNLIT_SIZE)
                .package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE)
                .build(*gEngine);
            vzmMaterials.push_back(gEngineApp->CreateMaterial("_DEFAULT_STANDARD_MATERIAL", material, nullptr, true)->GetVID());

            material = Material::Builder()
                .package(RESOURCES_SANDBOXUNLIT_DATA, RESOURCES_SANDBOXUNLIT_SIZE)
                .build(*gEngine);
            vzmMaterials.push_back(gEngineApp->CreateMaterial("_DEFAULT_UNLIT_MATERIAL", material, nullptr, true)->GetVID());

            material = Material::Builder()
                .package(FILAMENTAPP_TRANSPARENTCOLOR_DATA, FILAMENTAPP_TRANSPARENTCOLOR_SIZE)
                .build(*gEngine);
            vzmMaterials.push_back(gEngineApp->CreateMaterial("_DEFAULT_TRANSPARENT_MATERIAL", material, nullptr, true)->GetVID());

            gMaterialTransparent = material;
        }

        auto& ncm = VzNameCompManager::Get();
        gEngineApp->Initialize();

        return VZ_OK;
    }

    VZRESULT DeinitEngineLib()
    {
        if (safeReleaseChecker.get() == nullptr)
        {
            vzm::backlog::post("MUST CALL vzm::InitEngineLib before calling vzm::DeinitEngineLib()", backlog::LogLevel::Error);
            return VZ_WARNNING;
        }

        auto& ncm = VzNameCompManager::Get();
        // system unlock
        for (auto& it : vzmMaterials)
        {
            std::string name = ncm.GetName(utils::Entity::import(it));
            vzm::backlog::post("material (" + name + ") has been system-unlocked.", backlog::LogLevel::Default);
            VzMaterialRes* m_res = gEngineApp->GetMaterialRes(it);
            assert(m_res);
            m_res->isSystem = false;
        }

        for (auto& it : vzmGeometries)
        {
            std::string name = ncm.GetName(utils::Entity::import(it));
            vzm::backlog::post("geometry (" + name + ") has been system-unlocked.", backlog::LogLevel::Default);
            VzGeometryRes* geo_res = gEngineApp->GetGeometryRes(it);
            assert(geo_res);
            geo_res->isSystem = false;
        }

        gEngineApp->Destroy();

        if (gDummySwapChain) {
            gEngine->destroy(gDummySwapChain);
            gDummySwapChain = nullptr;
        }

        gMaterialProvider->destroyMaterials();
        delete gMaterialProvider;
        gMaterialProvider = nullptr;

        delete& ncm;

        //destroy all views belonging to renderPaths before destroy the engine 
        // note 
        // gEngine involves mJobSystem
        // when releasing gEngine, mJobSystem will be released!!
        // this calls JobSystem::requestExit() that includes JobSystem::wakeAll()
        Engine::destroy(&gEngine); // calls FEngine::shutdown()
        
        if (gVulkanPlatform) {
            delete gVulkanPlatform;
            gVulkanPlatform = nullptr;
        }

        delete gEngineApp;
        gEngineApp = nullptr;

        safeReleaseChecker->destroyed = true;
        safeReleaseChecker.reset();
        return VZ_OK;
    }

    VZRESULT ReleaseWindowHandlerTasks(void* window)
    {
        CHECK_API_VALIDITY(VZ_FAIL);
        if (window == nullptr)
        {
            return VZ_OK;
        }
        gEngineApp->CancelAyncLoad();

        std::vector<RendererVID> renderpath_vids;
        gEngineApp->GetRenderPathVids(renderpath_vids);
        for (RendererVID vid_renderpath : renderpath_vids)
        {
            VzRenderPath* render_path = gEngineApp->GetRenderPath(vid_renderpath);
            void* window_render_path = nullptr;
            render_path->GetCanvas(nullptr, nullptr, nullptr, &window_render_path);
            if (window == window_render_path)
            {
                gEngineApp->RemoveComponent(vid_renderpath);
            }
        }

        return VZ_OK;
    }

    VID GetFirstVidByName(const std::string& name)
    {
        CHECK_API_VALIDITY(INVALID_VID);
        return gEngineApp->GetFirstVidByName(name);
    }

    size_t GetVidsByName(const std::string& name, std::vector<VID>& vids)
    {
        CHECK_API_VALIDITY(0);
        return gEngineApp->GetVidsByName(name, vids);
    }

    bool GetNameByVid(const VID vid, std::string& name)
    {
        CHECK_API_VALIDITY(false);
        name = gEngineApp->GetNameByVid(vid);
        return name != "";
    }

    void RemoveComponent(const VID vid)
    {
        CHECK_API_VALIDITY( );
        auto& ncm = VzNameCompManager::Get();
        std::string name = ncm.GetName(utils::Entity::import(vid));
        if (gEngineApp->RemoveComponent(vid))
        {
            backlog::post("Component (" + name + ") has been removed", backlog::LogLevel::Default);
        }
        else
        {
            backlog::post("Invalid VID : " + std::to_string(vid), backlog::LogLevel::Error);
        }
    }

    VzScene* NewScene(const std::string& sceneName)
    {
        CHECK_API_VALIDITY(nullptr);
        return gEngineApp->CreateScene(sceneName);
    }

    VzRenderer* NewRenderer(const std::string& rendererName)
    {
        CHECK_API_VALIDITY(nullptr);
        return gEngineApp->CreateRenderPath(rendererName);
    }

    VzSceneComp* NewSceneComponent(const SCENE_COMPONENT_TYPE compType, const std::string& compName, const VID parentVid)
    {
        CHECK_API_VALIDITY(nullptr);
        VzSceneComp* v_comp = nullptr;
        v_comp = gEngineApp->CreateSceneComponent(compType, compName);
        if (v_comp == nullptr)
        {
            backlog::post("NewSceneComponent >> failure to gEngineApp->CreateSceneComponent", backlog::LogLevel::Error);
            return nullptr;
        }

        if (parentVid != 0)
        {
            gEngineApp->AppendSceneEntityToParent(v_comp->GetVID(), parentVid);
        }

        return v_comp;
    }

    VzResource* NewResComponent(const RES_COMPONENT_TYPE compType, const std::string& compName)
    {
        CHECK_API_VALIDITY(nullptr);
        VzResource* v_comp = nullptr;
        switch (compType)
        {
        case RES_COMPONENT_TYPE::GEOMATRY:
            v_comp = gEngineApp->CreateGeometry(compName, {}); break;
        case RES_COMPONENT_TYPE::MATERIAL:
            v_comp = gEngineApp->CreateMaterial(compName, {}); break;
        case RES_COMPONENT_TYPE::MATERIALINSTANCE:
            v_comp = gEngineApp->CreateMaterialInstance(compName, {}); break;
        case RES_COMPONENT_TYPE::TEXTURE:
            v_comp = gEngineApp->CreateTexture(compName, {}); break;
        case RES_COMPONENT_TYPE::FONT:
            v_comp = gEngineApp->CreateFont(compName); break;
        default:
            backlog::post("INVALID RESOURCE TYPE", backlog::LogLevel::Error);
        }
        if (v_comp == nullptr)
        {
            backlog::post("NewResComponent >> failure to gEngineApp->Create[ResComp]", backlog::LogLevel::Error);
            return nullptr;
        }
        return v_comp;
    }

    VID AppendSceneCompVidTo(const VID vid, const VID parentVid)
    {
        CHECK_API_VALIDITY(INVALID_VID);
        if (!gEngineApp->AppendSceneEntityToParent(vid, parentVid))
        {
            return INVALID_VID;
        }
        Scene* scene = gEngineApp->GetScene(parentVid);
        if (scene)
        {
            return parentVid;
        }
        return gEngineApp->GetSceneVidBelongTo(parentVid);
    }

    VzScene* AppendSceneCompTo(const VZ_NONNULL VzBaseComp* comp, const VZ_NULLABLE VzBaseComp* parentComp)
    {
        CHECK_API_VALIDITY(nullptr);
        return (VzScene*) GetVzComponent(AppendSceneCompVidTo(comp->GetVID(), parentComp ? parentComp->GetVID() : 0));
    };

    VzBaseComp* GetVzComponent(const VID vid)
    {
        CHECK_API_VALIDITY(nullptr);
        return gEngineApp->GetVzComponent<VzBaseComp>(vid);
    }

    VzBaseComp* GetFirstVzComponentByName(const std::string& name)
    {
        CHECK_API_VALIDITY(nullptr);
        return GetVzComponent(GetFirstVidByName(name));
    }

    size_t GetVzComponentsByName(const std::string& name, std::vector<VzBaseComp*>& components)
    {
        CHECK_API_VALIDITY(0);
        std::vector<VID> vids;
        size_t n = GetVidsByName(name, vids);
        if (n > 0) {
            components.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                components.push_back(GetVzComponent(vids[i]));
            }
        }
        return components.size();
    }

    size_t GetVzComponentsByType(const std::string& type, std::vector<VzBaseComp*>& components)
    {
        CHECK_API_VALIDITY(0);
        return gEngineApp->GetVzComponentsByType(type, components);
    }

    size_t GetSceneCompoenentVids(const SCENE_COMPONENT_TYPE compType, const VID sceneVid, std::vector<VID>& vids, const bool isRenderableOnly)
    {
        CHECK_API_VALIDITY(0);
        Scene* scene = nullptr;
        if (sceneVid != 0)
        {
            scene = gEngineApp->GetScene(sceneVid);
            if (scene == nullptr)
            {
                return 0;
            }
        }

        auto& rcm = gEngine->getRenderableManager();

        switch (compType)
        {
        case SCENE_COMPONENT_TYPE::CAMERA:
        {
            std::vector<VID> engine_vids;
            gEngineApp->GetCameraVids(engine_vids);
            for (auto& vid : engine_vids)
            {
                if (sceneVid == 0 || gEngineApp->GetSceneVidBelongTo(vid) == sceneVid)
                {
                    vids.push_back(vid);
                }
            }
            break;
        }
        case SCENE_COMPONENT_TYPE::ACTOR:
        {
            std::vector<VID> engine_vids;
            gEngineApp->GetActorVids(engine_vids);
            for (auto& vid : engine_vids)
            {
                if (sceneVid == 0 || gEngineApp->GetSceneVidBelongTo(vid) == sceneVid)
                {
                    if (isRenderableOnly)
                    {
                        Entity ett = Entity::import(vid);
                        if (rcm.hasComponent(ett))
                            vids.push_back(vid);
                    }
                    else
                    {
                        vids.push_back(vid);
                    }
                }
            }
            break;
        }
        case SCENE_COMPONENT_TYPE::LIGHT_SUN:
        case SCENE_COMPONENT_TYPE::LIGHT_POINT:
        case SCENE_COMPONENT_TYPE::LIGHT_DIRECTIONAL:
        case SCENE_COMPONENT_TYPE::LIGHT_FOCUSED_SPOT:
        case SCENE_COMPONENT_TYPE::LIGHT_SPOT:
        {
            std::vector<VID> engine_vids;
            gEngineApp->GetLightVids(engine_vids);
            for (auto& vid : engine_vids)
            {
                if (sceneVid == 0 || gEngineApp->GetSceneVidBelongTo(vid) == sceneVid)
                {
                    vids.push_back(vid);
                }
            }
            break;
        }
        default: break;
        }
        return vids.size();
    }
    
    VzActor* LoadTestModelIntoActor(const std::string& modelName)
    {
        CHECK_API_VALIDITY(nullptr);
        return gEngineApp->CreateTestActor(modelName);
    }

    VzActor* LoadModelFileIntoActors(const std::string& filename, std::vector<VzActor*>& actors)
    {
        CHECK_API_VALIDITY(nullptr);
        gEngineApp->LoadMeshFile(filename, actors);
        return actors.size() > 0 ? actors[0] : nullptr;
    }

    static std::ifstream::pos_type getFileSize(const char* filename) {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }
    filament::gltfio::FilamentAsset* loadAsset(const utils::Path& filename) {

        filament::gltfio::FilamentAsset* asset = nullptr;

        // Peek at the file size to allow pre-allocation.
        long const contentSize = static_cast<long>(getFileSize(filename.c_str()));
        if (contentSize <= 0) {
            backlog::post("Unable to open " + std::string(filename.c_str()), backlog::LogLevel::Error);
            return nullptr;
        }

        // Consume the glTF file.
        std::ifstream in(filename.c_str(), std::ifstream::binary | std::ifstream::in);
        std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
        if (!in.read((char*)buffer.data(), contentSize)) {
            backlog::post("Unable to read " + std::string(filename.c_str()), backlog::LogLevel::Error);
            return nullptr;
        }

        // Parse the glTF file and create Filament entities.
        VzAssetLoader* asset_loader = gEngineApp->GetGltfAssetLoader();
        asset = asset_loader->createAsset(buffer.data(), buffer.size());
        if (!asset) {
            backlog::post("Unable to parse " + std::string(filename.c_str()), backlog::LogLevel::Error);
            return nullptr;
        }

        buffer.clear();
        buffer.shrink_to_fit();
        return asset;
    };

    VzAsset* LoadFileIntoAsset(const std::string& filename, const std::string& assetName)
    {
        CHECK_API_VALIDITY(nullptr);
        utils::Path path = filename;
        filament::gltfio::FilamentAsset* asset = nullptr;
        VzAssetLoader* asset_loader = gEngineApp->GetGltfAssetLoader();
        asset_loader->mAssetName = assetName;
        // assume one instance per each asset (possibly multi-instance)
        if (path.isEmpty()) {
            asset = asset_loader->createAsset(
                GLTF_DEMO_DAMAGEDHELMET_DATA,
                GLTF_DEMO_DAMAGEDHELMET_SIZE);
        }
        else {
            asset = loadAsset(filename);
        }
        if (asset == nullptr)
        {
            backlog::post("asset loading failed!" + filename, backlog::LogLevel::Error);
            return nullptr;
        }

        filament::gltfio::FFilamentAsset* fasset = downcast(asset);
        
        std::set<TextureVID> tex_vids;
        for (auto& it : asset_loader->mTextureMap)
        {
            tex_vids.insert(it.second);
        }

        size_t num_m = asset_loader->mMaterialMap.size();
        size_t num_mi = asset_loader->mMIMap.size();
        size_t num_tex = tex_vids.size();// asset_loader->mTextureMap.size();
        size_t num_geo = asset_loader->mGeometryMap.size();
        size_t num_renderable = asset_loader->mRenderableActorMap.size();
        size_t num_node = asset_loader->mNodeActorMap.size();
        size_t num_camera = asset_loader->mCameraMap.size();
        size_t num_light = asset_loader->mLightMap.size();
        size_t num_skeleton = asset_loader->mSkeltonRootMap.size();
        size_t num_ins = fasset->mInstances.size();
        backlog::post(std::to_string(num_m) + " system-owned material" + (num_m > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_mi) + " material instance" + (num_mi > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_tex) + " texture" + (num_tex > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_geo) + (num_geo > 1 ? " geometries are" : " geometry is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_renderable) + " renderable actor" + (num_renderable > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_node) + " node actor" + (num_node > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_camera) + " camera" + (num_camera > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_light) + " light" + (num_light > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_skeleton) + " skeleton" + (num_skeleton > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);
        backlog::post(std::to_string(num_ins) + " gltf instance" + (num_ins > 1 ? "s are" : " is") + " created", backlog::LogLevel::Default);

#if !defined(__EMSCRIPTEN__)
        for (auto& it : asset_loader->mMaterialMap) {

            Material* ma = (Material*)it.first;
            // Don't attempt to precompile shaders on WebGL.
            // Chrome already suffers from slow shader compilation:
            // https://github.com/google/filament/issues/6615
            // Precompiling shaders exacerbates the problem.
            // First compile high priority variants
            ma->compile(Material::CompilerPriorityQueue::HIGH,
                UserVariantFilterBit::DIRECTIONAL_LIGHTING |
                UserVariantFilterBit::DYNAMIC_LIGHTING |
                UserVariantFilterBit::SHADOW_RECEIVER);

            // and then, everything else at low priority, except STE, which is very uncommon.
            ma->compile(Material::CompilerPriorityQueue::LOW,
                UserVariantFilterBit::FOG |
                UserVariantFilterBit::SKINNING |
                UserVariantFilterBit::SSR |
                UserVariantFilterBit::VSM);
        }
#endif
        VzAsset* v_asset = gEngineApp->CreateAsset(assetName);
        AssetVID vid_asset = v_asset->GetVID();
        VzAssetRes& asset_res = *gEngineApp->GetAssetRes(vid_asset);
        asset_res.animator = VzAsset::Animator(vid_asset);
        asset_res.asset = asset;
        asset_res.asyncTextures = asset_loader->mTextureMap;

        // from asset components
        {
#define RegisterFromAsset1(A, B) for (auto& it : B) { A.insert(it.first); }
#define RegisterFromAsset2(A, B) for (auto& it : B) { A.insert(it.second); }

            RegisterFromAsset1(asset_res.fromAssetLights, asset_loader->mLightMap);
            RegisterFromAsset1(asset_res.fromAssetCameras, asset_loader->mCameraMap);
            RegisterFromAsset1(asset_res.fromAssetRenderableActors, asset_loader->mRenderableActorMap);
            RegisterFromAsset1(asset_res.fromAssetNodes, asset_loader->mNodeActorMap);
            RegisterFromAsset1(asset_res.fromAssetSketetons, asset_loader->mSkeltonRootMap);

            RegisterFromAsset2(asset_res.fromAssetGeometries, asset_loader->mGeometryMap);
            RegisterFromAsset2(asset_res.fromAssetMaterials, asset_loader->mMaterialMap);
            RegisterFromAsset2(asset_res.fromAssetMIs, asset_loader->mMIMap);
            RegisterFromAsset2(asset_res.fromAssetTextures, asset_loader->mTextureMap);
        }

        for (auto& instance : fasset->mInstances)
        {
            asset_res.rootVIDs.push_back(instance->mRoot.getId());
        }
#if 0
        for (auto& it : asset_loader->mSkeltonRootMap)
        {
            asset_res.skeletons.push_back(it.first);
            std::vector<BoneVID> bone_vids;
            bone_vids.push_back(it.first);
            getDescendants(it.first, bone_vids);

            gEngineApp->CreateSkeleton(it.second, it.first);
            VzSkeletonRes* skeleton_res = gEngineApp->GetSkeletonRes(it.first);
            
            skeleton_res->bones.clear();
            size_t num_bones = bone_vids.size();
            skeleton_res->bones.reserve(num_bones);
            for (size_t i = 0; i < num_bones; ++i)
            {
                BoneVID vid_bone = bone_vids[i];
                skeleton_res->bones[i] = vid_bone;
                asset_res.assetOwnershipComponents.insert(vid_bone);
            }
            //asset_res.assetOwnershipComponents.insert(it.first); // already involved
        }
#endif

        ResourceConfiguration configuration = {};
        configuration.engine = gEngine;
        configuration.gltfPath = path.c_str();
        configuration.normalizeSkinningWeights = true;

        ResourceLoader* resource_loader = gEngineApp->GetGltfResourceLoader();
        resource_loader->setConfiguration(configuration);
        if (!resource_loader->asyncBeginLoad(asset)) {
            asset_loader->destroyAsset((filament::gltfio::FFilamentAsset*)asset);
            backlog::post("Unable to start loading resources for " + filename, backlog::LogLevel::Error);
            return nullptr;
        }
#if 0
        auto createAnimator = [&]() {
            VzAnimator* animator = gEngineApp->CreateAnimator("animator_" + assetName);
            asset_res.animatorVID = animator->GetVID();
            VzAniRes* ani_res = gEngineApp->GetAniRes(asset_res.animatorVID);
            ani_res->assetOwner = asset;

            assert(fasset->mInstances.size() > 0);
            filament::gltfio::FFilamentInstance* instance = fasset->mInstances[0];

            assert(fasset->mResourcesLoaded && fasset->mSourceAsset);
            auto& impl = ani_res->animator;
            impl = new skm::AnimatorImpl();
            impl->animatorVID = animator->GetVID();

            impl->skins.resize(instance->mSkins.size());
            for (size_t i = 0, len = instance->mSkins.size(); i < len; ++i)
            {
                const FFilamentInstance::Skin& srcSkin = instance->mSkins[i];
                skm::Skeleton& dstSkin = impl->skins[i];
                dstSkin.targets = srcSkin.targets;
                dstSkin.joints = srcSkin.joints;
            }

            impl->rootEntity = instance->mRoot;
            impl->renderableManager = &fasset->mEngine->getRenderableManager();
            impl->transformManager = &fasset->mEngine->getTransformManager();
            impl->trsTransformManager = fasset->getTrsTransformManager();

            const cgltf_data* srcAsset = fasset->mSourceAsset->hierarchy;
            const cgltf_animation* srcAnims = srcAsset->animations;
            for (cgltf_size i = 0, len = srcAsset->animations_count; i < len; ++i)
            {
                const cgltf_animation& anim = srcAnims[i];
                if (!skm::validateAnimation(anim))
                {
                    backlog::post("Disabling animation due to validation failure.",
                                  backlog::LogLevel::Warning);
                    return;
                }
            }

            // Loop over the glTF animation definitions.
            impl->animations.resize(srcAsset->animations_count);
            for (cgltf_size i = 0, len = srcAsset->animations_count; i < len; ++i)
            {
                const cgltf_animation& srcAnim = srcAnims[i];
                skm::Animation& dstAnim = impl->animations[i];
                dstAnim.duration = 0;
                if (srcAnim.name)
                {
                    dstAnim.name = srcAnim.name;
                }

                // Import each glTF sampler into a custom data structure.
                cgltf_animation_sampler* srcSamplers = srcAnim.samplers;
                dstAnim.samplers.resize(srcAnim.samplers_count);
                for (cgltf_size j = 0, nsamps = srcAnim.samplers_count; j < nsamps; ++j)
                {
                    const cgltf_animation_sampler& srcSampler = srcSamplers[j];
                    skm::Sampler& dstSampler = dstAnim.samplers[j];
                    skm::createSampler(srcSampler, dstSampler);
                    if (dstSampler.times.size() > 1)
                    {
                        float maxtime = (--dstSampler.times.end())->first;
                        dstAnim.duration = std::max(dstAnim.duration, maxtime);
                    }
                }

                // Import each glTF channel into a custom data structure.
                const cgltf_node* nodes = fasset->mSourceAsset->hierarchy->nodes;
                if (instance)
                {
                    impl->addChannels(instance->mNodeMap, nodes, srcAnim, dstAnim);
                }
                else
                {
                    for (FFilamentInstance* instance : fasset->mInstances)
                    {
                        impl->addChannels(instance->mNodeMap, nodes, srcAnim, dstAnim);
                    }
                }
            }
        };

        createAnimator();
#endif
        gEngineApp->activeAsyncAsset = vid_asset;

        //auto& rcm = gEngine->getRenderableManager();
        //auto& lcm = gEngine->getLightManager();
        //auto& tcm = gEngine->getTransformManager();

        //asset->releaseSourceData();

        // Enable stencil writes on all material instances.
        filament::gltfio::FilamentInstance* asset_ins = asset->getInstance();
        const size_t mi_count = asset_ins->getMaterialInstanceCount();
        MaterialInstance* const* const mis = asset_ins->getMaterialInstances();
        for (int mi = 0; mi < mi_count; mi++) {
            mis[mi]->setStencilWrite(true);
            mis[mi]->setStencilOpDepthStencilPass(MaterialInstance::StencilOperation::INCR);
        } 

        return v_asset;
    }
    
    void ExportAssetToGlb(const VZ_NONNULL VzAsset* v_asset, const std::string& filename)
    {
        CHECK_API_VALIDITY( );
        AssetVID vid_asset = v_asset->GetVID();
        VzAssetRes& asset_res = *gEngineApp->GetAssetRes(vid_asset);

        filament::gltfio::VzAssetExpoter* asset_exporter = gEngineApp->GetGltfAssetExpoter();
        asset_exporter->ExportToGlb(v_asset, filename);
    }

    float GetAsyncLoadProgress()
    {
        CHECK_API_VALIDITY(-1.f);
        ResourceLoader* resource_loader = gEngineApp->GetGltfResourceLoader();
        if (resource_loader == nullptr)
        {
            backlog::post("resource loader is not activated!", backlog::LogLevel::Error);
            return -1.f;
        }
        return resource_loader->asyncGetLoadProgress();
    }

    void ReloadShader()
    {
        CHECK_API_VALIDITY( );
        //wi::renderer::ReloadShaders();
    }

    VID DisplayEngineProfiling(const int w, const int h, const bool displayProfile, const bool displayEngineStates)
    {
        CHECK_API_VALIDITY(INVALID_VID);
        return 0;
    }

    void* GetGraphicsSharedRenderTarget() 
    {
        CHECK_API_VALIDITY(nullptr);
        return gEngine->getSwapHandle();
    }
}
