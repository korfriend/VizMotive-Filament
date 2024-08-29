// Filament highlevel APIs
#include "VizEngineAPIs.h"

#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <X11/Xlib.h> 
#endif

//#include "glm/glm.hpp"

// Main code
int main(int, char**)
{

  if (!glfwInit()) return 1;

  // Setting the graphics API is done by the engine.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


  GLFWwindow* window =
      glfwCreateWindow(800, 400, "Grapicar GLFW Emulator", nullptr, nullptr);

  if (!window) return 1;

  vzm::ParamMap<std::string> arguments;

  arguments.SetParam("api", std::string("opengl"));

  vzm::InitEngineLib(arguments);
  vzm::VzScene* scene = vzm::NewScene("my scene");
  //scene->LoadIBL("../../../VisualStudio/samples/assets/ibl/lightroom_14b");
  vzm::VzRenderer* renderer = vzm::NewRenderer("my renderer");
#ifdef _WIN32
    renderer->SetCanvas(800, 400, 96.f, glfwGetWin32Window(window));
#endif
#ifdef __linux__
    renderer->SetCanvas(800, 400, 96.f, (void*)(glfwGetX11Window(window)));
#endif
  renderer->SetVisibleLayerMask(0x4, 0x4);
  //vzm::VzActor* actor = vzm::LoadTestModelIntoActor("my actor");
   //vzm::VzAsset* asset = vzm::LoadFileIntoAsset("../assets/cube.gltf", "my gltf asset");

  int w = 800;
  int h = 400;

  vzm::VzCamera* cam = (vzm::VzCamera*)vzm::NewSceneComponent(
  vzm::SCENE_COMPONENT_TYPE::CAMERA, "my camera", 0);
  float p[3] = {0, 0, 10};
  float at[3] = {0, 0, -4 };
  float u[3] = {0, 1, 0};
  cam->SetWorldPose((float*)&p, (float*)&at, (float*)&u);
  cam->SetPerspectiveProjection(0.1f, 1000.f, 45.f, (float)w / (float)h);
  cam->SetMatrixAutoUpdate(false);
  vzm::VzCamera::Controller* cc = cam->GetController();
  *(float**)cc->orbitHomePosition = p;
  cc->UpdateControllerSettings();
  cc->SetViewport(w, h);

  // vzm::VzLight* light = (vzm::VzLight*)vzm::NewSceneComponent(
  //     vzm::SCENE_COMPONENT_TYPE::LIGHT, "my light");
  //  vzm::AppendSceneCompVidTo(actor->GetVID(), scene->GetVID());
  // vzm::AppendSceneCompTo(light, scene);
  vzm::AppendSceneCompTo(cam, scene);

  //if (asset){
  //std::vector<VID> root_vids = asset->GetGLTFRoots();
  //  if (root_vids.size() > 0) {
  //      vzm::AppendSceneCompVidTo(root_vids[0], scene->GetVID());
  //  }
  //}

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    renderer->Render(scene, cam);


  }

  vzm::DeinitEngineLib();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0; 
}
