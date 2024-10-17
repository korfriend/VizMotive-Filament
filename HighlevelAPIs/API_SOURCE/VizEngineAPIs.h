///
/// @file      VizEngineAPIs.h
/// @brief     The VizEngineAPIs header file.
/// @date      2024-10-16
/// @author    Engine Team
/// @copyright GrapiCar Inc. All Rights Reserved.

#ifndef HIGHLEVELAPIS_API_SOURCE_VIZENGINEAPIS_H_
#define HIGHLEVELAPIS_API_SOURCE_VIZENGINEAPIS_H_

#include "VzComponents.h"

namespace vzm {

/// @fn IsEngineAvailable
/// @brief
/// Check if the engine is available.
/// @return
/// true if the engine is available, false otherwise.
extern "C" API_EXPORT bool IsEngineAvailable();

/// @fn InitEngineLib
/// @brief
/// Initialize the engine library.
/// @param[in] arguments
/// The arguments.
/// @return
/// The result.
extern "C" API_EXPORT VZRESULT InitEngineLib(
    const vzm::ParamMap<std::string>& arguments = vzm::ParamMap<std::string>());

/// @fn DeinitEngineLib
/// @brief
/// Deinitialize the engine library.
/// @return
/// The result.
extern "C" API_EXPORT VZRESULT DeinitEngineLib();

/// @fn ReleaseWindowHandlerTasks
/// @brief
/// Release window handler tasks.
/// @param[in] window
/// The window.
/// @return
/// The result.
extern "C" API_EXPORT VZRESULT ReleaseWindowHandlerTasks(void* window);

/// @fn GetFirstVidByName
/// @brief
/// Get the first entity ID whose name is the input name.
/// @param[in] name
/// The name.
/// @return
/// The entity ID.
extern "C" API_EXPORT VID GetFirstVidByName(const std::string& name);

/// @fn GetVidsByName
/// @brief
/// Get Entity IDs whose name is the input name.
/// @param[in] name
/// The name.
/// @param[out] vids
/// The entity IDs.
/// @return
/// The number of entities.
extern "C" API_EXPORT size_t GetVidsByName(const std::string& name,
                                           std::vector<VID>& vids);

/// @fn GetNameByVid
/// @brief
/// Get Entity's name if possible.
/// @param[in] vid
/// The entity ID.
/// @param[out] name
/// The entity's name.
/// @return
/// true if the entity's name exists, false otherwise.
extern "C" API_EXPORT bool GetNameByVid(const VID vid, std::string& name);

/// @fn RemoveComponent
/// @brief
/// Remove an entity.
/// @param[in] vid
/// The entity ID.
extern "C" API_EXPORT void RemoveComponent(const VID vid);

/// @fn NewScene
/// @brief
/// Create a new scene.
/// @param[in] sceneName
/// The scene name.
/// @return
/// The scene.
extern "C" API_EXPORT VzScene* NewScene(const std::string& sceneName);

/// @fn NewRenderer
/// @brief
/// Create a new renderer.
/// @param[in] rendererName
/// The renderer name.
/// @return
/// The renderer.
extern "C" API_EXPORT VzRenderer* NewRenderer(const std::string& rendererName);

/// @fn NewSceneComponent
/// @brief
/// Create a new scene component.
/// @param[in] compType
/// The component type.
/// @param[in] compName
/// The component name.
/// @param[in] parentVid
/// The parent entity ID.
/// @return
/// The scene component.
extern "C" API_EXPORT VzSceneComp* NewSceneComponent(
    const SCENE_COMPONENT_TYPE compType, const std::string& compName,
    const VID parentVid = 0u);

/// @fn NewResComponent
/// @brief
/// Create a new resource component.
/// @param[in] compType
/// The component type.
/// @param[in] compName
/// The component name.
/// @return
/// The resource component.
extern "C" API_EXPORT VzResource* NewResComponent(
    const RES_COMPONENT_TYPE compType, const std::string& compName);

/// @fn GetVzComponent
/// @brief
/// Get a component.
/// @param[in] vid
/// The entity ID.
/// @return
/// The component.
extern "C" API_EXPORT VzBaseComp* GetVzComponent(const VID vid);

/// @fn GetFirstVzComponentByName
/// @brief
/// Get the first component whose name is the input name.
/// @param[in] name
/// The name.
/// @return
/// The component.
extern "C" API_EXPORT VzBaseComp* GetFirstVzComponentByName(
    const std::string& name);

/// @fn GetVzComponentsByName
/// @brief
/// Get components whose name is the input name.
/// @param[in] name
/// The name.
/// @param[out] components
/// The components.
/// @return
/// The number of components.
extern "C" API_EXPORT size_t GetVzComponentsByName(
    const std::string& name, std::vector<VzBaseComp*>& components);

/// @fn GetVzComponentsByType
/// @brief
/// Get components whose type is the input type.
/// @param[in] type
/// The type.
/// @param[out] components
/// The components.
/// @return
/// The number of components.
extern "C" API_EXPORT size_t GetVzComponentsByType(
    const std::string& type, std::vector<VzBaseComp*>& components);

/// @fn AppendSceneCompVidTo
/// @brief
/// Append a scene component to another scene component.
/// @param[in] vid
/// The entity ID.
/// @param[in] parentVid
/// The parent entity ID.
/// @return
/// The scene entity ID.
extern "C" API_EXPORT VID AppendSceneCompVidTo(const VID vid,
                                               const VID parentVid);

/// @fn AppendSceneCompTo
/// @brief
/// Append a scene component to another scene component.
/// @param[in] comp
/// The component.
/// @param[in] parentComp
/// The parent component.
/// @return
/// The scene.
extern "C" API_EXPORT VzScene* AppendSceneCompTo(
    const VZ_NONNULL VzBaseComp* comp,
    const VZ_NULLABLE VzBaseComp* parentComp);

/// @fn GetSceneCompoenentVids
/// @brief
/// Get component IDs in a scene.
/// @param[in] compType
/// The component type.
/// @param[in] sceneVid
/// The scene entity ID.
/// @param[out] vids
/// The component IDs.
/// @param[in] isRenderableOnly
/// true if only renderable components are needed, false otherwise.
/// @return
/// The number of components.
extern "C" API_EXPORT size_t GetSceneCompoenentVids(
    const SCENE_COMPONENT_TYPE compType, const VID sceneVid,
    std::vector<VID>& vids, const bool isRenderableOnly = false);

/// @fn LoadTestModelIntoActor
/// @brief
/// Load a system actor and return the actor.
/// @param[in] modelName
/// The model name.
/// @return
/// The actor.
extern "C" API_EXPORT VzActor* LoadTestModelIntoActor(
    const std::string& modelName);

/// @fn LoadModelFileIntoActors
/// @brief
/// Load a mesh file (obj and stl) into actors and return the first actor.
/// @param[in] filename
/// The file name.
/// @param[out] actors
/// The actors.
/// @return
/// The actor.
extern "C" API_EXPORT VzActor* LoadModelFileIntoActors(
    const std::string& filename, std::vector<VzActor*>& actors);

/// @fn LoadFileIntoAsset
/// @brief
/// Load a file into an asset.
/// @param[in] filename
/// The file name.
/// @param[in] assetName
/// The asset name.
/// @return
/// The asset.
extern "C" API_EXPORT VzAsset* LoadFileIntoAsset(const std::string& filename,
                                                 const std::string& assetName);

/// @fn GetAsyncLoadProgress
/// @brief
/// Get the progress of asynchronous loading.
/// @return
/// The progress.
extern "C" API_EXPORT float GetAsyncLoadProgress();

/// @fn GetGraphicsSharedRenderTarget
/// @brief
/// Get a graphics render target view.
/// @return
/// The graphics render target view.
extern "C" API_EXPORT void* GetGraphicsSharedRenderTarget();

/// @fn ReloadShader
/// @brief
/// Reload the shader.
extern "C" API_EXPORT void ReloadShader();

/// @fn DisplayEngineProfiling
/// @brief
/// Display engine profiling.
/// @param[in] w
/// The width.
/// @param[in] h
/// The height.
/// @param[in] displayProfile
/// true if the profile is displayed, false otherwise.
/// @param[in] displayEngineStates
/// true if the engine states are displayed, false otherwise.
/// @return
/// The display ID.
extern "C" API_EXPORT VID DisplayEngineProfiling(
    const int w, const int h, const bool displayProfile = true,
    const bool displayEngineStates = true);

/// @fn ExportAssetToGlb
/// @brief
/// Export an asset to a glb file.
/// @param[in] asset
/// The asset.
/// @param[in] filename
/// The file name.
extern "C" API_EXPORT void ExportAssetToGlb(const VZ_NONNULL VzAsset* asset,
                                            const std::string& filename);

}  // namespace vzm

#endif  // HIGHLEVELAPIS_API_SOURCE_VIZENGINEAPIS_H_
