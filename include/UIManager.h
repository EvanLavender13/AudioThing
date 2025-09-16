#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "VisualizerConfig.h"
#include <imgui.h>

class UIManager {
public:
  UIManager(VisualizerConfig &config);
  ~UIManager();

  void drawUI(float fps, float frameTime);

private:
  VisualizerConfig &config; // Reference to the shared configuration
};

#endif // UI_MANAGER_H
