#include "main_content.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <android/log.h>

#define G_DEBUG(...)       __android_log_print(ANDROID_LOG_DEBUG, __FILE__, __VA_ARGS__)
#define G_WARN(...)        __android_log_print(ANDROID_LOG_WARN, __FILE__, __VA_ARGS__)
#define G_ERROR(...)       __android_log_print(ANDROID_LOG_ERROR, __FILE__, __VA_ARGS__)

 
contents::MainContent mainContent;

#ifdef __cplusplus
extern "C" {
#endif
void setUp(const int width, const int height, void* window) {
  mainContent.setRootPath("../../../");
  mainContent.setUp(width, height, window);
}
void runMainLoop() {
  while (true) {
    mainContent.render();
  }

  mainContent.terminate();
}
#ifdef __cplusplus
}
#endif

namespace contents {

void MainContent::setUp(const int width, const int height, void* window) {
  vzm::ParamMap<std::string> arguments;

  arguments.SetParam("api", std::string("opengl"));

  G_DEBUG("in setUP() %s : %x", __FUNCTION__, std::this_thread::get_id());
  vzm::InitEngineLib(arguments);

  width_ = width;
  height_ = height;
  window_ = window;

  if ((!root_path_.empty()) && (width_ > 0) && (height_ > 0)) {
    _init();
  }
}

bool MainContent::_init() {
  vzm::VzAsset* asset = nullptr;
  vzm::VzAsset::Animator* animator = nullptr;

  if (!renderer_) {
    renderer_ = vzm::NewRenderer("my renderer");
  }

  renderer_->SetCanvas(width_, height_, 96.0F, window_);
  renderer_->SetVisibleLayerMask(0x4, 0x4);

  if (!scene_) {
    scene_ = vzm::NewScene("my scene");
  }

  scene_->LoadIBL(root_path_ + "VisualStudio/samples/assets/ibl/lightroom_14b");

  asset = vzm::LoadFileIntoAsset(
      root_path_ + "HighlevelAPIs/Samples/assets/Soldier.glb", "my gltf asset");

  if(asset) {
  animator = asset->GetAnimator();
  }

  if (animator) {
    animator->AddPlayScene(scene_->GetVID());
    animator->SetPlayMode(vzm::VzAsset::Animator::PlayMode::PLAY);
    animator->ActivateAnimation(1);
  }

  if (!camera_) {
    vzm::VzCamera::Controller* cc;
    glm::fvec3 p(0, 0, 10);
    glm::fvec3 at(0, 0, -4);
    glm::fvec3 u(0, 1, 0);

    camera_ = (vzm::VzCamera*)vzm::NewSceneComponent(vzm::SCENE_COMPONENT_TYPE::CAMERA,
                                                     "mycamera", 0);

    camera_->SetWorldPose((float*)&p, (float*)&at, (float*)&u);
    camera_->SetPerspectiveProjection(0.1f, 1000.f, 45.f, (float)width_ / (float)height_);
    camera_->SetMatrixAutoUpdate(false);

    cc = camera_->GetController();

    *(glm::fvec3*)cc->orbitHomePosition = p;
    cc->UpdateControllerSettings();
    cc->SetViewport(width_, height_);

    vzm::AppendSceneCompTo(camera_, scene_);
  }

  if (asset) {
    std::vector<VID> root_vids = asset->GetGLTFRoots();

    if (root_vids.size() > 0) {
      vzm::AppendSceneCompVidTo(root_vids[0], scene_->GetVID());
    }
  }

  is_initialized_ = true;

  return is_initialized_;
}

void MainContent::render() {
  if ((!is_paused_) && (is_initialized_ || _init())) {
    renderer_->Render(scene_, camera_);
  }
}

void MainContent::terminate() {
  vzm::DeinitEngineLib();

  renderer_ = nullptr;
  scene_ = nullptr;
  camera_ = nullptr;

  window_ = nullptr;

  is_initialized_ = false;
}

void MainContent::play() {
  is_paused_= false;
}

void MainContent::pause() {
  is_paused_ = true;
}

void MainContent::mouseDown(const int id, const int button,
                            const float x, const float y) {
  if (camera_) {
    if (id == 0) {
      camera_->GetController()->GrabBegin((int)x, (int)y, false);
    } else if (id == 1) {
      camera_->GetController()->GrabBegin((int)x, (int)y, true);
    }
  }
}

void MainContent::mouseUp(const int id, const int button,
                          const float x, const float y) {
  if (camera_ && ((id == 0) || (id == 1))) {
    camera_->GetController()->GrabEnd();
  }
}

void MainContent::mouseMove(const int id, const float x, const float y) {
  if (camera_ && ((id == 0) || (id == 1))) {
    camera_->GetController()->GrabDrag((int)x, (int)y);
  }
}

void MainContent::scroll(const float x, const float y, const float delta) {
  if (camera_) {
    camera_->GetController()->Scroll((int)x, (int)y, delta);
  }
}

void MainContent::keyDown(const int key) {

}

void MainContent::keyUp(const int key) {

}

bool MainContent::isInitialized() const {
  return is_initialized_;
}

bool MainContent::isPaused() const {
  return is_paused_;
}

void MainContent::setRootPath(const std::string& root_path) {
  root_path_ = root_path;
}

int MainContent::getWidth() const {
  return width_;
}

int MainContent::getHeight() const {
  return height_;
}

void* MainContent::getWidhow() const {
  return window_;
}

std::string MainContent::getRootPath() const {
  return root_path_;
}

}  // namespace contents
