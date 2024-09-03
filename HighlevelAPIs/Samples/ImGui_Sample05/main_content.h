#ifndef MAIN_CONTENT_H_
#define MAIN_CONTENT_H_

#include "VizEngineAPIs.h"

namespace contents {

class MainContent {
 public:
  MainContent() = default;
  ~MainContent() = default;

  void setUp(const int width, const int height, void* window);
  void render();
  void terminate();

  void play();
  void pause();

  void mouseDown(const int id, const int button, const float x, const float y);
  void mouseUp(const int id, const int button, const float x, const float y);
  void mouseMove(const int id, const float x, const float y);

  void scroll(const float x, const float y, const float delta);

  void keyDown(const int key);
  void keyUp(const int key);

  bool isInitialized() const;
  bool isPaused() const;

  void setRootPath(const std::string& root_path);

  int getWidth() const;
  int getHeight() const;
  void* getWidhow() const;
  std::string getRootPath() const;

 private:
  bool _init();

  std::string root_path_ = "";
  vzm::VzRenderer* renderer_ = nullptr;
  vzm::VzScene* scene_ = nullptr;
  vzm::VzCamera* camera_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  void* window_ = nullptr;
  bool is_paused_ = false;
  bool is_initialized_ = false;
};

} // namespace contents

#endif // MAIN_CONTENT_H_
