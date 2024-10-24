#pragma once

#include "PreDefs.h"

#include <filament/Engine.h>
#include <filament/LightManager.h>
#include <filament/Camera.h>
#include <filament/Frustum.h>
#include <filament/Viewport.h>
#include <filament/Material.h>
#include <filament/Renderer.h>
#include <filament/SwapChain.h>
#include <filament/RenderableManager.h>
#include <filament/MaterialInstance.h>
#include <filament/TransformManager.h>
#include <filament/Scene.h>
#include <filament/View.h>
#include <filament/Skybox.h>
#include <filament/ColorSpace.h>

#include <utils/EntityManager.h>
#include <utils/EntityInstance.h>
#include <utils/NameComponentManager.h>
#include <utils/JobSystem.h>
#include <utils/Path.h>
#include <utils/Systrace.h>
#include <utils/BitmaskEnum.h>
#include <utils/Log.h>

#include <gltfio/AssetLoader.h>
#include <gltfio/FilamentAsset.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <gltfio/NodeManager.h>
#include <gltfio/Animator.h>
#include <gltfio/math.h>

#include <filamat/MaterialBuilder.h>

#include <filameshio/MeshReader.h>

#include "backend/VzConfig.h"
#include "backend/VzCube.h"
#include "backend/VzIBL.h"


#include "backend/resource_internal.h"
#include "generated/resources/resources.h"
#include "generated/resources/monkey.h"
#include "generated/resources/gltf_demo.h"
//#include "../../VisualStudio/libs/filamentapp/generated/resources/filamentapp.h"
#include "materials/uberarchive.h"

// FEngine
#include "../../libs/gltfio/src/FFilamentAsset.h"
#include "../../libs/gltfio/src/FNodeManager.h"
#include "../../libs/gltfio/src/extended/AssetLoaderExtended.h"
#include "../../libs/gltfio/src/FTrsTransformManager.h"
#include "../../libs/gltfio/src/GltfEnums.h"

#include "../../filament/src/details/Engine.h"
#include "../../filament/src/ResourceAllocator.h"
#include "../../filament/src/components/RenderableManager.h"

using namespace filament;
using namespace filamesh;
using namespace filament::math;
using namespace filament::backend;
using namespace filament::gltfio;
using namespace utils;
using namespace filament::color;
using namespace filamat;
