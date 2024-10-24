// Filament highlevel APIs
#include <GLFW/glfw3.h>

#include "VizEngineAPIs.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __linux__
#define GLFW_EXPOSE_NATIVE_X11
#include <X11/Xlib.h>
#endif
#include <GLFW/glfw3native.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <iostream>

vzm::VzCamera* g_cam = nullptr;

void setMouseButton(GLFWwindow* window, int button, int state,
                    int modifier_key) {
  if (!g_cam) {
    return;
  }
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);

  switch (state) {
    case GLFW_PRESS:
      if (button == 0) {
        g_cam->GetController()->GrabBegin(x, y, false);
      } else if (button == 1) {
        g_cam->GetController()->GrabBegin(x, y, true);
      }
      break;
    case GLFW_RELEASE:
      g_cam->GetController()->GrabEnd();
      break;
    default:
      break;
  }
}
void setCursorPos(GLFWwindow* window, double x, double y) {
  if (!g_cam) {
    return;
  }
  if (glfwGetMouseButton(window, 0) == GLFW_PRESS ||
      glfwGetMouseButton(window, 1) == GLFW_PRESS) {
    g_cam->GetController()->GrabDrag(x, y);
  }
}
void setMouseScroll(GLFWwindow* window, double xOffset, double yOffset) {
  if (!g_cam) {
    return;
  }
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);

  g_cam->GetController()->Scroll(static_cast<int>(x), static_cast<int>(y),
                                 -10.0f * (float)yOffset);
}

// Main code
int main(int, char**) {
  int w = 800;
  int h = 400;

  if (!glfwInit()) return 1;

  // Setting the graphics API is done by the engine.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow* window =
      glfwCreateWindow(w, h, "Grapicar GLFW Emulator", nullptr, nullptr);

  if (!window) return 1;

  glfwSetMouseButtonCallback(window, setMouseButton);
  glfwSetCursorPosCallback(window, setCursorPos);
  glfwSetScrollCallback(window, setMouseScroll);

  vzm::ParamMap<std::string> arguments;

  arguments.SetParam("api", std::string("opengl"));

  vzm::InitEngineLib(arguments);
  vzm::VzScene* soldierScene = vzm::NewScene("soldier scene");

  vzm::VzCompositor* compositor = vzm::NewCompositor("my compositor");
#ifdef _WIN32
  compositor->SetCanvas(w, h, 96.f, glfwGetWin32Window(window));
  soldierScene->LoadIBL(
      "../../../VisualStudio/samples/assets/ibl/lightroom_14b");
  vzm::VzAsset* asset = vzm::LoadFileIntoAsset("../assets/Soldier.glb", "my gltf asset");
#elif __linux__
  compositor->SetCanvas(w, h, 96.f, (void*)(glfwGetX11Window(window)));
#ifdef _DEBUG
  soldierScene->LoadIBL(
      "../../../../out/debug/filament/bin/assets/ibl/lightroom_14b");
#else
  soldierScene->LoadIBL(
      "../../../../out/release/filament/bin/assets/ibl/lightroom_14b");
#endif
  vzm::VzAsset* asset =
      vzm::LoadFileIntoAsset("../../assets/Soldier.glb", "my gltf asset");
#endif

  vzm::VzAsset::Animator* animator = asset->GetAnimator();
  if (animator) {
    animator->AddPlayScene(soldierScene->GetVID());
    animator->SetPlayMode(vzm::VzAsset::Animator::PlayMode::PLAY);
    animator->ActivateAnimation(1);
  }

  {
    vzm::VzSpriteActor* sprite = (vzm::VzSpriteActor*)vzm::NewSceneComponent(
        vzm::SCENE_COMPONENT_TYPE::SPRITE_ACTOR,
        "my sprite");
    sprite->SetSpriteWidth(2.0f)
        .SetSpriteHeight(2.0f)
        .SetAnchorU(0.5)
        .SetAnchorV(0.5)
        .Build();
    vzm::VzTexture* texture = (vzm::VzTexture*)vzm::NewResComponent(
        vzm::RES_COMPONENT_TYPE::TEXTURE, "my image 1");

#ifdef _WIN32
    texture->ReadImage("../assets/testimage.png");
#elif __linux__
    texture->ReadImage("../../assets/testimage.png");
#endif
    sprite->SetTexture(texture->GetVID());
    glm::fvec3 sprite_p = glm::fvec3(0.0f, 2.0f, 0.0f);
    sprite->SetPosition(__FP sprite_p);
    sprite->EnableBillboard(true);
    sprite->SetVisibleLayer(vzm::VzBaseActor::VISIBLE_LAYER::GUI);
    vzm::AppendSceneCompTo(sprite, soldierScene);
  }

  vzm::VzCamera* fixedCam = (vzm::VzCamera*)vzm::NewSceneComponent(
      vzm::SCENE_COMPONENT_TYPE::CAMERA, "fixed camera", 0);
  glm::fvec3 fixed_p(0, 3, -5);
  glm::fvec3 fixed_at(0, 0, 4);
  glm::fvec3 fixed_u(0, 1, 0);
  fixedCam->SetWorldPose((float*)&fixed_p, (float*)&fixed_at, (float*)&fixed_u);
  fixedCam->SetPerspectiveProjection(0.1f, 1000.f, 45.f, (float)w / (float)h);
  fixedCam->SetMatrixAutoUpdate(false);

  g_cam = (vzm::VzCamera*)vzm::NewSceneComponent(
      vzm::SCENE_COMPONENT_TYPE::CAMERA, "mycamera", 0);
  glm::fvec3 p(0, 0, 10);
  glm::fvec3 at(0, 0, -4);
  glm::fvec3 u(0, 1, 0);
  g_cam->SetWorldPose((float*)&p, (float*)&at, (float*)&u);
  g_cam->SetPerspectiveProjection(0.1f, 1000.f, 45.f, (float)w / (float)h);
  g_cam->SetMatrixAutoUpdate(false);
  vzm::VzCamera::Controller* cc = g_cam->GetController();
  *(glm::fvec3*)cc->orbitHomePosition = p;
  cc->UpdateControllerSettings();
  cc->SetViewport(w, h);

  vzm::AppendSceneCompTo(fixedCam, soldierScene);
  vzm::AppendSceneCompTo(g_cam, soldierScene);

  vzm::VzScene* testScene = vzm::NewScene("test scene");
  vzm::VzActor* testActor = vzm::LoadTestModelIntoActor("my test model");
  vzm::VzCamera* testCam = (vzm::VzCamera*)vzm::NewSceneComponent(
      vzm::SCENE_COMPONENT_TYPE::CAMERA, "fixed camera", 0);
  glm::fvec3 test_p(0, 0, 10);
  glm::fvec3 test_at(0, 0, -4);
  glm::fvec3 test_u(0, 1, 0);
  testCam->SetWorldPose((float*)&test_p, (float*)&test_at, (float*)&test_u);
  testCam->SetPerspectiveProjection(0.1f, 1000.f, 45.f, (float)w / (float)h);
  testCam->SetMatrixAutoUpdate(false);
  vzm::VzSunLight* testLight = (vzm::VzSunLight*)vzm::NewSceneComponent(
      vzm::SCENE_COMPONENT_TYPE::LIGHT_SUN, "sun");
  testLight->SetIntensity(1000000.0f);
  vzm::AppendSceneCompTo(testActor, testScene);
  vzm::AppendSceneCompTo(testLight, testScene);
  vzm::AppendSceneCompTo(testCam, testScene);


  if (asset) {
    std::vector<VID> root_vids = asset->GetGLTFRoots();
    if (root_vids.size() > 0) {
      vzm::AppendSceneCompVidTo(root_vids[0], soldierScene->GetVID());
    }
  }

  vzm::CompositorLayer* layer1 =
      compositor->AddLayer(soldierScene->GetVID(), g_cam->GetVID(), 
                       vzm::VzBaseActor::VISIBLE_LAYER::VISIBLE);
  layer1->SetViewport(0, 0, w / 2, h / 2);

  vzm::CompositorLayer* testSceneLayer =
      compositor->AddLayer(testScene->GetVID(), testCam->GetVID(),
                       vzm::VzBaseActor::VISIBLE_LAYER::VISIBLE);
  testSceneLayer->SetViewport(w / 2, 0, w / 2, h / 2);

  vzm::CompositorLayer* postprocess_layer =
      compositor->AddLayer(soldierScene->GetVID(), g_cam->GetVID(),
                       vzm::VzBaseActor::VISIBLE_LAYER::VISIBLE);
  postprocess_layer->SetViewport(0, h / 2, w / 2, h / 2);

  postprocess_layer->SetPostProcessingEnabled(true);
  postprocess_layer->SetBloomEnabled(true);
  postprocess_layer->SetBloomLensFlare(true);
  postprocess_layer->SetBloomStrength(20.0f);
  postprocess_layer->SetFogEnabled(true);
  float fogColor[3] = {0.7f, 0.3f, 0.2f};
  postprocess_layer->SetFogColor(fogColor);
  postprocess_layer->SetTaaEnabled(true);
  postprocess_layer->SetVignetteEnabled(true);
  
  vzm::CompositorLayer* fixedCamLayer =
      compositor->AddLayer(soldierScene->GetVID(), fixedCam->GetVID(),
                       vzm::VzBaseActor::VISIBLE_LAYER(0x3));
  fixedCamLayer->SetViewport(w / 2, h / 2, w / 2, h / 2);

  vzm::CompositorLayer* guiLayer = compositor->AddLayer(soldierScene->GetVID(), g_cam->GetVID(),
                       vzm::VzBaseActor::VISIBLE_LAYER::GUI);
  guiLayer->SetViewport(0, 0, w, h);

  compositor->SetLayerOrder(postprocess_layer, 4);

  //layer1->ApplyViewSettingsFrom(postprocess_layer);
  //compositor->RemoveLayer(fixedCamLayer);
  
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    compositor->Render();
  }

  vzm::DeinitEngineLib();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
