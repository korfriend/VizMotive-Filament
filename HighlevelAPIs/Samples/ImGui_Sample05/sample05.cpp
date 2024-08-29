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
  vzm::VzScene* scene = vzm::NewScene("my scene");

  vzm::VzRenderer* renderer = vzm::NewRenderer("my renderer");
#ifdef _WIN32
  renderer->SetCanvas(w, h, 96.f, glfwGetWin32Window(window));
  scene->LoadIBL("../../../VisualStudio/samples/assets/ibl/lightroom_14b");
  vzm::VzAsset* asset = vzm::LoadFileIntoAsset("../assets/Soldier.glb", "my gltf asset");
#elif __linux__
  renderer->SetCanvas(w, h, 96.f, (void*)(glfwGetX11Window(window)));
  scene->LoadIBL("../../../../VisualStudio/samples/assets/ibl/lightroom_14b");
  vzm::VzAsset* asset =
      vzm::LoadFileIntoAsset("../../assets/Soldier.glb", "my gltf asset");
#endif
  renderer->SetVisibleLayerMask(0x4, 0x4);

  vzm::VzAsset::Animator* animator = asset->GetAnimator();
  if (animator) {
    animator->AddPlayScene(scene->GetVID());
    animator->SetPlayMode(vzm::VzAsset::Animator::PlayMode::PLAY);
    animator->ActivateAnimation(1);

    std::vector<std::string> animations = animator->GetAnimationLabels();
    std::cout << "Total animations: " << animations.size() << std::endl;
    std::cout << "Animation names:" << std::endl;
    for (const auto& animName : animations) {
      std::cout << "  - " << animName << std::endl;
    }
  }

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

  vzm::AppendSceneCompTo(g_cam, scene);

  if (asset) {
    std::vector<VID> root_vids = asset->GetGLTFRoots();
    if (root_vids.size() > 0) {
      vzm::AppendSceneCompVidTo(root_vids[0], scene->GetVID());
    }
  }

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    renderer->Render(scene, g_cam);
  }

  vzm::DeinitEngineLib();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
